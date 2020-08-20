#ifndef LITEFLOW_FIREWALL_H
#define LITEFLOW_FIREWALL_H

#ifndef __KERNEL__
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#else
#include <net/genetlink.h>
#endif

#define LF_FIREWALL_APP_ID 2 // Firewall applications will be given ID 2

#define INPUT_METRICS_POS_XXX1 0
#define INPUT_METRICS_POS_XXX2 1
// More can be added
#define NUM_OF_INPUT_VALUE 2

#define OUTPUT_XXX 0
#define NUM_OF_OUTPUT_VALUE 1

#endif