#ifndef LITEFLOW_NL_H
#define LITEFLOW_NL_H

#include <linux/kernel.h>

#ifndef __KERNEL__
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#else
#include <net/genetlink.h>
#endif

/* -- netlink related --- */
#define LF_NL_NAME "lf"
#define LF_NL_VERSION 1

#define LF_NL_MC_DEFAULT_NAME "lf_default"

enum lf_multicast_groups {
    LF_NL_MC_DEFAULT, // Start from 0
    __LF_NL_MC_MAX,
};
#define LF_NL_MC_MAX (__LF_NL_MC_MAX - 1) 

enum lf_controls {
    LF_NL_C_UNSPEC,
    LF_NL_C_ACTIVATE_MODEL,
    LF_NL_C_ACTIVATE_MODEL_RET,
    LF_NL_C_REPORT_MODEL_ACTIVATION,
    __LF_NL_C_MAX,
};
#define LF_NL_C_MAX (__LF_NL_C_MAX - 1)

enum lf_attrs {
    LF_NL_ATTR_UNSPEC,
    LF_NL_ATTR_APP_ID,
    LF_NL_ATTR_MODEL_ID,
    LF_NL_ATTR_RET_CODE,
    __LF_NL_ATTR__MAX,
};
#define LF_NL_ATTR_MAX (__LF_NL_ATTR__MAX - 1)
#ifdef __KERNEL__
static const struct nla_policy lf_policy[LF_NL_ATTR_MAX + 1] = {
#else 
static struct nla_policy lf_policy[LF_NL_ATTR_MAX + 1] = {
#endif
    [LF_NL_ATTR_APP_ID] = {
        .type = NLA_U8,
    },
    [LF_NL_ATTR_MODEL_ID] = {
        .type = NLA_U32,
    },
    [LF_NL_ATTR_RET_CODE] = {
        .type = NLA_U8,
    }
};

struct lf_nl_ops {
    int (*recv_activation_cb)(__u8 appid, __u32 model_uuid);
};

extern int start_nl(struct lf_nl_ops *);
extern int stop_nl(void);
extern int report_model_activation(__u8 appid, __u32 model_uuid);

#endif