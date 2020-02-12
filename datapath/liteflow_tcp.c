// LiteFlow TCP Kernel will register to both LiteFlow kernek and Kernel TCP Congestion Control
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/net.h>
#include <net/tcp.h>
#include <linux/inet_diag.h>

#include "linux/liteflow.h"

#define LF_TCP_APP_ID 1 // CC applications will be given ID 1
// Here we allow different connection can use different models, just change the APP_ID per connection
// In this implementation, connection information is put in the input of the NN

// NUM_OF_INPUT_METRICS metrics will be passed to NN
// such as acked bytes, ecn bytes and so on
#define INPUT_METRICS_POS_BYTES_ACKED 0
#define INPUT_METRICS_POS_ECN_BYTES 1
// More can be added
#define NUM_OF_INPUT_METRICS 2

#define HISTORY_LEN 3

#define OUTPUT_RATE 0
#define NUM_OF_OUTPUT_VALUE 1

struct lf_tcp_metric {
    s64 values[NUM_OF_INPUT_METRICS]; // store collected metrics
};

struct lf_tcp_internal {
    u64 src_ip;
    u64 dest_ip;
    u64 src_port;
    u64 dest_port;
    u64 last_snd_una;
    u32 current_pointer;
    struct lf_tcp_metric* metrics; // ring buffer
};

static inline void lf_increase_pointer(struct lf_tcp_internal *lf_tcp) {
    u32 current_pointer;

    current_pointer = lf_tcp->current_pointer;
    lf_tcp->current_pointer = (++current_pointer) % HISTORY_LEN;
}

// Set rate towards connection
static inline void lf_set_rate (struct sock *sk, u32 rate) {
    sk->sk_pacing_rate = rate;
}

static void lf_tcp_conn_init(struct sock *sk) {
    struct lf_tcp_internal *ca;
    struct tcp_sock *tp;

    printk(KERN_INFO "New flow handled by liteflow tcp kernel inits...\n");
    ca = inet_csk_ca(sk);
    tp = tcp_sk(sk);

    ca->src_ip = tp->inet_conn.icsk_inet.inet_saddr;
    ca->src_port = tp->inet_conn.icsk_inet.inet_sport;
    ca->dest_ip = tp->inet_conn.icsk_inet.inet_daddr;
    ca->dest_port = tp->inet_conn.icsk_inet.inet_dport;
    ca->last_snd_una = tp->snd_una;

    ca->current_pointer = 0;
    ca->metrics = kmalloc(sizeof(struct lf_tcp_metric) * HISTORY_LEN, GFP_KERNEL);
    memset(ca->metrics, 0, sizeof(struct lf_tcp_metric) * HISTORY_LEN);

    if (!(tp->ecn_flags & TCP_ECN_OK)) {
        INET_ECN_dontxmit(sk);
    }
    // Turn on pacing, so we can use pacing to control the speed
    // Learn from BBR and CCP :)
    cmpxchg(&sk->sk_pacing_status, SK_PACING_NONE, SK_PACING_NEEDED);
}

static void lf_tcp_conn_release(struct sock *sk)
{
    printk(KERN_INFO "Free flow handled by liteflow tcp kernel...\n");
}

// Learn from CCP
static u32 lf_tcp_conn_ssthresh(struct sock *sk) 
{
    const struct tcp_sock *tp = tcp_sk(sk);
    return max(tp->snd_cwnd >> 1U, 2U);
}

// Learn from CCP
static u32 lf_tcp_conn_undo_cwnd(struct sock *sk) 
{
    const struct tcp_sock *tp = tcp_sk(sk);
    return max(tp->snd_cwnd, tp->snd_ssthresh << 1);
}

// Send all metrics to liteflow hosted NN
// and set sending rate based on returned rate
static void lf_tcp_conn_nn_control(struct sock *sk, const struct rate_sample *rs) 
{
    int ret, metric_pos, value_pos, pos = 0;
    u32 output_rate;
    struct lf_tcp_internal *ca;
    s64 nn_input[NUM_OF_INPUT_METRICS * HISTORY_LEN];
    s64 nn_output[NUM_OF_OUTPUT_VALUE];
    
    ca = inet_csk_ca(sk);
    if(ca->metrics == NULL) {
        printk(KERN_ERR "Current flow is not managed by liteflow tcp kernel!\n");
        return;
    }
    
    // Prepare input vector
    // Performance is not good
    for (metric_pos = 0; metric_pos < HISTORY_LEN; ++metric_pos) {
        for (value_pos = 0; value_pos < NUM_OF_INPUT_METRICS; ++value_pos) {
            nn_input[pos] = ca->metrics[(ca->current_pointer + metric_pos) % HISTORY_LEN].values[value_pos];
            pos++;
        }
    }

    ret = lf_query_model(LF_TCP_APP_ID, nn_input, nn_output);
    if (ret == LF_ERROR) {
        printk(KERN_ERR "Query NN model failed!\n");
    } else {
        output_rate = nn_output[OUTPUT_RATE];
        lf_set_rate(sk, output_rate);
    }
}

static void lf_tcp_conn_in_ack_event(struct sock *sk, u32 flags) 
{
    // Obtain information
    // More can be added
    const struct tcp_sock *tp;
    struct lf_tcp_internal *ca;
    u32 acked_bytes;
    
    tp = tcp_sk(sk);
    ca = inet_csk_ca(sk);

    if(ca->metrics == NULL) {
        printk(KERN_ERR "Current flow is not managed by liteflow tcp kernel!\n");
        return;
    }

    acked_bytes = tp->snd_una - ca->last_snd_una;
    ca->last_snd_una = tp->snd_una;

    ca->metrics[ca->current_pointer].values[INPUT_METRICS_POS_BYTES_ACKED] = acked_bytes;
    if (flags & CA_ACK_ECE) {
        ca->metrics[ca->current_pointer].values[INPUT_METRICS_POS_ECN_BYTES] = acked_bytes;
    } else {
        ca->metrics[ca->current_pointer].values[INPUT_METRICS_POS_ECN_BYTES] = 0;
    }
    lf_increase_pointer(ca);
}


struct app lf_tcp_app = {
    .appid = LF_TCP_APP_ID,
    .input_size = NUM_OF_INPUT_METRICS,
    .output_size = NUM_OF_OUTPUT_VALUE,
};

struct tcp_congestion_ops lf_tcp_congestion_ops = {
    .init = lf_tcp_conn_init,
    .release = lf_tcp_conn_release,

    .ssthresh = lf_tcp_conn_ssthresh,
    .undo_cwnd = lf_tcp_conn_undo_cwnd,
    // .cong_avoid = lf_tcp_conn_cong_avoid,
    .cong_control = lf_tcp_conn_nn_control,
    // .set_state = ,
    // .pkts_acked = ，
    .in_ack_event = lf_tcp_conn_in_ack_event,
    .owner = THIS_MODULE,
    .name = "lf_tcp_kernel",
};

static int
__init liteflow_tcp_kernel_init(void)
{
    int ret;

    BUILD_BUG_ON(sizeof(struct lf_tcp_internal) > ICSK_CA_PRIV_SIZE);
    ret = tcp_register_congestion_control(&lf_tcp_congestion_ops);
    if (ret != 0) {
        printk(KERN_ERR "Cannot register liteflow tcp kernel with kernel CC!\n");
        return ret;
    }
    ret = lf_register_app(&lf_tcp_app);
    if (ret != LF_SUCCS) {
        printk(KERN_ERR "Cannot register liteflow tcp kernel with liteflow kernel!\n");
        return ret;
    }
    printk(KERN_INFO "Successfully register liteflow tcp kernel with liteflow kernel and kernel CC...\n");
    return ret;
}

static void
__exit liteflow_tcp_kernel_exit(void)
{
    tcp_unregister_congestion_control(&lf_tcp_congestion_ops);
    lf_unregister_app(LF_TCP_APP_ID);
}

module_init(liteflow_tcp_kernel_init);
module_exit(liteflow_tcp_kernel_exit);

MODULE_DESCRIPTION("liteflow tcp kernel");
MODULE_AUTHOR("Junxue ZHANG");
MODULE_LICENSE("GPL v2");