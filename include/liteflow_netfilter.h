#ifndef LITEFLOW_NETFILTER_H
#define LITEFLOW_NETFILTER_H

#ifndef __KERNEL__
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#else
#include <net/genetlink.h>
#endif

#define LF_NETFILTER_APP_ID 2 // Netfilter applications will be given ID 2

#define INPUT_METRICS_POS_SRC_IP_A 0
#define INPUT_METRICS_POS_SRC_IP_B 1
#define INPUT_METRICS_POS_SRC_IP_C 2
#define INPUT_METRICS_POS_SRC_IP_D 3
#define INPUT_METRICS_POS_SRC_PORT 4
#define INPUT_METRICS_POS_DST_IP_A 5
#define INPUT_METRICS_POS_DST_IP_B 6
#define INPUT_METRICS_POS_DST_IP_C 7
#define INPUT_METRICS_POS_DST_IP_D 8
#define INPUT_METRICS_POS_DST_PORT 9
// More can be added
#define NUM_OF_INPUT_VALUE 10

#define OUTPUT_SHOULD_PASS 0
#define OUTPUT_SHOULD_DROP 1
#define NUM_OF_OUTPUT_VALUE 2

#endif