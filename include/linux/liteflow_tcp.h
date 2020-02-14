#ifndef LITEFLOW_TCP_H
#define LITEFLOW_TCP_H

#define LF_TCP_APP_ID 1 // CC applications will be given ID 1
// Here we allow different connection can use different models, just change the APP_ID per connection
// In this implementation, connection information is put in the input of the NN

#define LF_NL_NAME "LITEFLOW_TCP"
#define LF_NL_VERSION 1

enum {
    LF_NL_C_UNSPEC,
    LF_NL_C_SEND, // Kernel send
    __LF_NL_C_MAX,
};
#define LF_NL_C_MAX (__LF_NL_K_MAX - 1)

// NUM_OF_INPUT_METRICS metrics in a history record
// such as acked bytes, ecn bytes and so on
#define INPUT_METRICS_POS_BYTES_ACKED 0
#define INPUT_METRICS_POS_PACKETS_ACKED 1
#define INPUT_METRICS_POS_ECN_BYTES 2
#define INPUT_METRICS_POS_ECN_PACKETS 3
#define INPUT_METRICS_POS_RTT_SAMPLE_US 4
#define INPUT_METRICS_POS_DELIVERED 5
#define INPUT_METRICS_POS_INTERVAL_US 6
#define INPUT_METRICS_POS_RATE_OUTGOING 7
// More can be added
#define NUM_OF_INPUT_METRICS 8

#define HISTORY_LEN 3

// NUM_OF_GLOBAL_STATS per connection
#define GLOBAL_STATS_POS_MIN_RTT_US 0
// More can be added
#define NUM_OF_GLOBAL_STATS 1

#define OUTPUT_RATE 0
#define NUM_OF_OUTPUT_VALUE 1

#endif