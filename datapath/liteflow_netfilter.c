// LiteFlow Netfilter Kernel will register to both LiteFlow kernek and Kernel IPV4 Netfilter
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>

#include "linux/liteflow.h"
#include "liteflow_netfilter.h"

static unsigned int
hook_func_incoming(void *priv,
		struct sk_buff *skb,
		const struct nf_hook_state *state)
{
	int ret;
	struct iphdr *ip_header;
	struct tcphdr *tcp_header;
	__be32 saddr, daddr;
	__be16 sport, dport;
	s64 nn_input[NUM_OF_INPUT_VALUE];
	s64 nn_output[NUM_OF_OUTPUT_VALUE];

	ip_header = ip_hdr(skb);

	if (ip_header -> protocol == IPPROTO_TCP) {
		tcp_header = tcp_hdr(skb);
	} else {
		return NF_ACCEPT; // Non TCP traffic will not be processed
	}

	saddr = ntohl(ip_header->saddr);
	daddr = ntohl(ip_header->daddr);
	sport = ntohs(tcp_header->source);
	dport = ntohs(tcp_header->dest);

	nn_input[INPUT_METRICS_POS_SRC_IP_A_B] = (saddr >> 16) & 0xffff;;
	nn_input[INPUT_METRICS_POS_SRC_IP_C_D] = (saddr >> 0) & 0xffff;

	nn_input[INPUT_METRICS_POS_DST_IP_A_B] = (daddr >> 16) & 0xffff;
	nn_input[INPUT_METRICS_POS_DST_IP_C_D] = (daddr >> 0) & 0xffff;

	nn_input[INPUT_METRICS_POS_SRC_PORT] = sport;
	nn_input[INPUT_METRICS_POS_DST_PORT] = dport;

	ret = lf_query_model(LF_NETFILTER_APP_ID, nn_input, nn_output);
    if (ret == LF_ERROR) {
        printk(KERN_ERR "Query NN model failed!\n");
    } else {
		if (nn_output[OUTPUT_SHOULD_PASS] > nn_output[OUTPUT_SHOULD_DROP])
			return NF_ACCEPT;
		else
			return NF_DROP;
    }

    return NF_ACCEPT;
}

static unsigned int
hook_func_outcoming(void *priv,
	struct sk_buff *skb,
	const struct nf_hook_state *state)
{
    return NF_ACCEPT;
}

static struct nf_hook_ops
lf_netfilter_ops_in __read_mostly = {
		hook: hook_func_incoming,
		hooknum: NF_INET_PRE_ROUTING,
		pf: PF_INET,
		priority: NF_IP_PRI_FIRST
};

static struct nf_hook_ops
lf_netfilter_ops_out __read_mostly = {
		hook: hook_func_outcoming,
		hooknum: NF_INET_POST_ROUTING,
		pf: PF_INET,
		priority: NF_IP_PRI_FIRST
};

struct app lf_netfilter_app = {
    .appid = LF_NETFILTER_APP_ID,
    .input_size = NUM_OF_INPUT_VALUE,
    .output_size = NUM_OF_OUTPUT_VALUE,
};

static int
__init liteflow_netfilter_kernel_init(void)
{
	int ret;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
    nf_register_net_hook(&init_net, &lf_netfilter_ops_in);
	nf_register_net_hook(&init_net, &lf_netfilter_ops_out);
#else
    nf_register_hook(&lf_netfilter_ops_in);
	nf_register_hook(&lf_netfilter_ops_out);
#endif

	ret = lf_register_app(&lf_netfilter_app);
    if (ret != LF_SUCCS) {
        printk(KERN_ERR "Cannot register liteflow netfilter kernel with liteflow kernel!\n");
        return ret;
    }

	return ret;
}

static void
__exit liteflow_netfilter_kernel_exit(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
	nf_unregister_net_hook(&init_net, &lf_netfilter_ops_out);
    nf_unregister_net_hook(&init_net, &lf_netfilter_ops_in);
#else
	nf_unregister_hook(&lf_netfilter_ops_out);
	nf_unregister_hook(&lf_netfilter_ops_in);
#endif
	
    lf_unregister_app(LF_NETFILTER_APP_ID);
}

module_init(liteflow_netfilter_kernel_init);
module_exit(liteflow_netfilter_kernel_exit);

MODULE_DESCRIPTION("liteflow netfilter kernel");
MODULE_AUTHOR("Junxue ZHANG");
MODULE_AUTHOR("Chaoliang ZENG");
MODULE_LICENSE("GPL v2");
