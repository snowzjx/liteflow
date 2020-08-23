#ifndef LITEFLOW_TCP_H
#define LITEFLOW_TCP_H

#ifndef __KERNEL__
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#else
#include <net/genetlink.h>
#endif

#define LF_TCP_APP_ID 1 // CC applications will be given ID 1
// Here we allow different connection can use different models, just change the APP_ID per connection
// In this implementation, connection information is put in the input of the NN

// Aurora input
#define INPUT_SCALE 1000
#define OUTPUT_SCALE 1000

// NUM_OF_INPUT_METRICS metrics in a history record
// Here we dedicate the design for PCC-RL
#define INPUT_METRICS_POS_SENT_LAT_INFLACTION 0
#define INPUT_METRICS_POS_LAT_RATIO 1
#define INPUT_METRICS_POS_SEND_RATIO 2

#define NUM_OF_INPUT_METRICS 3

#define HISTORY_LEN 10

#define INPUT_SIZE (NUM_OF_INPUT_METRICS * HISTORY_LEN)

// NUM_OF_GLOBAL_STATS per connection
#define GLOBAL_STATS_POS_MIN_RTT_US 0
// More can be added
#define NUM_OF_GLOBAL_STATS 1

#define OUTPUT_RATE 0
#define NUM_OF_OUTPUT_VALUE 1

/* -- netlink related --- */
#define LF_TCP_NL_NAME "lf_tcp"
#define LF_TCP_NL_VERSION 1

#define LF_TCP_NL_MC_DEFAULT_NAME "lf_tcp_default"

enum lf_tcp_multicast_groups {
    LF_TCP_NL_MC_DEFAULT, // Start from 0
    __LF_TCP_NL_MC_MAX,
};
#define LF_TCP_NL_MC_MAX (__LF_TCP_NL_MC_MAX - 1)

enum lf_tcp_controls {
    LF_TCP_NL_C_UNSPEC,
    LF_TCP_NL_C_REPORT, // Kernel send
    __LF_TCP_NL_C_MAX,
};
#define LF_TCP_NL_C_MAX (__LF_TCP_NL_C_MAX - 1)

enum lf_tcp_attrs {
    LF_TCP_NL_ATTR_UNSPEC,
    LF_TCP_NL_ATTR_NN_INPUT, // The data of NN
    __LF_TCP_NL_ATTR__MAX,
};
#define LF_TCP_NL_ATTR_MAX (__LF_TCP_NL_ATTR__MAX - 1)

#ifdef __KERNEL__
static const struct nla_policy lf_tcp_policy[LF_TCP_NL_ATTR_MAX + 1] = {
#else 
static struct nla_policy lf_tcp_policy[LF_TCP_NL_ATTR_MAX + 1] = {
#endif
    [LF_TCP_NL_ATTR_NN_INPUT] = {
        .type = NLA_UNSPEC,
#ifndef __KERNEL__
        .minlen = INPUT_SIZE * sizeof(__s64),
        .maxlen = INPUT_SIZE * sizeof(__s64),
#endif
    },
};


#endif
