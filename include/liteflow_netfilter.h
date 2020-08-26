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

#define INPUT_METRICS_POS_SRC_IP_A_B 0
#define INPUT_METRICS_POS_SRC_IP_C_D 1
#define INPUT_METRICS_POS_SRC_PORT 2
#define INPUT_METRICS_POS_DST_IP_A_B 3
#define INPUT_METRICS_POS_DST_IP_C_D 4
#define INPUT_METRICS_POS_DST_PORT 5
// More can be added
#define NUM_OF_INPUT_VALUE 6

#define OUTPUT_SHOULD_PASS 0
#define OUTPUT_SHOULD_DROP 1
#define NUM_OF_OUTPUT_VALUE 2

#endif