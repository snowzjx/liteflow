#ifndef LITEFLOW_NL_H
#define LITEFLOW_NL_H

#include <linux/module.h>
#include <linux/kernel.h>

struct lf_nl_ops {
    int (*recv_activation_cb)(u8 appid, u32 model_uuid);
};

extern int start_nl(struct lf_nl_ops *);

#endif