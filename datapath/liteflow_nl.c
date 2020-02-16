#include <net/genetlink.h>

#include "linux/liteflow.h"
#include "liteflow_nl.h"

static struct genl_family lf_gnl_family;

struct lf_nl_ops *default_ops = NULL;

static struct genl_multicast_group lf_mcgrps[] = {
    [LF_NL_MC_DEFAULT] = { .name = LF_NL_MC_DEFAULT_NAME, },
};

static int lf_gnl_rx_msg(struct sk_buff* skb, struct genl_info* info) {
    struct nlattr *app_id_attr, *model_id_attr;
    u8 appid;
    u32 model_uuid;
    struct sk_buff *rskb;
    void *msg_head;
    int ret;
    int lf_ret;

    if (default_ops == NULL) {
        printk(KERN_ERR "Default ops is NULL!\n");
        return LF_ERROR;
    }

    app_id_attr = info->attrs[LF_NL_ATTR_APP_ID];
    model_id_attr = info->attrs[LF_NL_ATTR_MODEL_ID];

    if (app_id_attr == NULL) {
        printk(KERN_ERR "Received NULL App ID attr!\n");
        return LF_ERROR;
    }
    if (model_id_attr == NULL) {
        printk(KERN_ERR "Received NULL Model ID attr!\n");
        return LF_ERROR;
    }

    appid = nla_get_u8(app_id_attr);
    model_uuid = nla_get_u32(model_id_attr);

    lf_ret = default_ops->recv_activation_cb (appid, model_uuid);
    
    // Reply
    rskb = genlmsg_new(NLMSG_GOODSIZE, GFP_ATOMIC);

    if (rskb == NULL) {
        printk (KERN_ERR "Cannot allocate skb for LiteFlow netlink ...\n");
        return LF_ERROR;
    }

    msg_head = genlmsg_put_reply(rskb, info, &lf_gnl_family, GFP_ATOMIC, LF_NL_C_ACTIVATE_MODEL_RET);
    if (msg_head == NULL) {
        printk (KERN_ERR "Cannot allocate msg_head for LiteFlow netlink ...\n");
        return LF_ERROR;
    }

    ret = nla_put_u8(skb, LF_NL_ATTR_RET_CODE, lf_ret);
    if (ret != 0) {
        printk (KERN_ERR "Cannot put data for LiteFlow netlink, error code: %d ...\n", ret);
        return LF_ERROR;
    }

    genlmsg_end(rskb, msg_head);
    
    ret = genlmsg_reply(rskb, info);
    if (ret != 0) {
        printk (KERN_ERR "Cannot reply meesage, error code: %d ...\n", ret);
        return LF_ERROR;
    }

    return LF_SUCCS;
}

static const struct genl_ops lf_ops[] = {
    {
        .cmd = LF_NL_C_ACTIVATE_MODEL,
        .policy = lf_policy,
        .doit = lf_gnl_rx_msg,
        .dumpit = NULL,
    },
};

static struct genl_family lf_gnl_family = {
    .name = LF_NL_NAME,     
    .version = LF_NL_VERSION,
    .netnsok = false,
    .maxattr = LF_NL_ATTR_MAX,
    .ops = lf_ops,
    .n_ops = ARRAY_SIZE(lf_ops),
    .mcgrps = lf_mcgrps,
    .n_mcgrps = ARRAY_SIZE(lf_mcgrps),
    .module = THIS_MODULE,
};

int start_nl(struct lf_nl_ops *ops) {
    int ret;

    printk(KERN_INFO "Starting liteflow netlink subsystem...\n");

    ret = genl_register_family(&lf_gnl_family);
    if (ret != 0) {
        printk(KERN_ERR "Cannot register liteflow generic netlink!\n");
        return LF_ERROR;
    }

    default_ops = ops;
    return LF_SUCCS;
}

int stop_nl(void) {
    int ret;

    printk(KERN_INFO "Stopping liteflow netlink subsystem...\n");

    default_ops = NULL;
    ret = genl_unregister_family(&lf_gnl_family);
    if (ret != 0) {
        printk(KERN_ERR "Cannot unregister liteflow generic netlink!\n");
        return LF_ERROR;
    }

    return LF_SUCCS;
}

int report_model_activation(u8 appid, u32 model_uuid) {
    struct sk_buff *skb;
    void *msg_head;
    int ret;

    skb = genlmsg_new(NLMSG_GOODSIZE, GFP_ATOMIC);

    if (skb == NULL) {
        printk (KERN_ERR "Cannot allocate skb for LiteFlow netlink ...\n");
        return LF_ERROR;
    }

    msg_head = genlmsg_put(skb, 0, 0, &lf_gnl_family, GFP_ATOMIC, LF_NL_C_REPORT_MODEL_ACTIVATION);
    if (msg_head == NULL) {
        printk (KERN_ERR "Cannot allocate msg_head for LiteFlow netlink ...\n");
        return LF_ERROR;
    }

    ret = nla_put_u8(skb, LF_NL_ATTR_APP_ID, appid);
    if (ret != 0) {
        printk (KERN_ERR "Cannot put data for LiteFlow netlink, error code: %d ...\n", ret);
        return LF_ERROR;
    }

    ret = nla_put_u32(skb, LF_NL_ATTR_MODEL_ID, model_uuid);
    if (ret != 0) {
        printk (KERN_ERR "Cannot put data for LiteFlow netlink, error code: %d ...\n", ret);
        return LF_ERROR;
    }

    genlmsg_end(skb, msg_head);

    genlmsg_multicast(&lf_gnl_family, skb, 0, LF_NL_MC_DEFAULT, GFP_ATOMIC);

    return LF_SUCCS;
}
