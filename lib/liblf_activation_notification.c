#include "liblf.h"

#include <stdbool.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>

#include "liteflow_nl.h"

struct ops_wrapper {
    bool (*ops)(__u8 appid, __u32 model_uuid);
    bool shoud_stop;
};

static int rx_msg(struct nl_msg *msg, void* args)
{
    struct nlattr *attr[LF_NL_ATTR_MAX + 1];
    struct genlmsghdr *ghdr;
    __u8 appid;
    __u32 model_uuid;
    struct ops_wrapper *wrapper = args;

    ghdr = genlmsg_hdr(nlmsg_hdr(msg));
    if(ghdr->cmd != LF_NL_C_REPORT_MODEL_ACTIVATION) {
        fprintf(stderr, "Not expected command...\n");
        return -1;
    }

    genlmsg_parse(nlmsg_hdr(msg), 0, attr, LF_NL_ATTR_MAX, lf_policy);

    if (!attr[LF_NL_ATTR_APP_ID]) {
        fprintf(stderr, "Empty message RX!\n");
        return NL_OK;
    }

    if (!attr[LF_NL_ATTR_MODEL_ID]) {
        fprintf(stderr, "Empty message RX!\n");
        return NL_OK;
    }

    appid = nla_get_u8(attr[LF_NL_ATTR_APP_ID]);
    model_uuid = nla_get_u32(attr[LF_NL_ATTR_MODEL_ID]);

    wrapper->shoud_stop = wrapper->ops(appid, model_uuid);
    
    return 0;
}

int rx_activation_notification(bool (*rx)(__u8 appid, __u32 model_uuid))
{
    struct nl_sock* sock = NULL;
    struct nl_cb *cb = NULL;
    int family_id, grp_id;
    struct ops_wrapper wrapper = {
        .ops = rx,
        .shoud_stop = false,
    };

    sock = connect_lf_genl_sock();
    if (sock == NULL) {
        return -1;
    }

    family_id = resolve_family_id(sock, LF_NL_NAME);
    if (family_id < 0) {
        return -1;
    }

    grp_id = resolve_grp_id(sock, LF_NL_NAME, LF_NL_MC_DEFAULT_NAME);
    if (grp_id < 0) {
        fprintf(stderr, "Unable to resolve group name...\n");
        return -1;
    }

    if (nl_socket_add_membership(sock, grp_id)) {
        fprintf(stderr, "Unable to join group %u!\n", grp_id); 
    }

    cb = nl_cb_alloc(NL_CB_DEFAULT);
    nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, rx_msg, &wrapper);

    do {
        nl_recvmsgs(sock, cb);
    } while (wrapper.shoud_stop == false);

    nl_cb_put(cb);
    nl_socket_free(sock);

    return 0;
}

// bool test_rx (__u8 appid, __u32 model_uuid)
// {
//     static int count = 0;
//     if (count > 3) {
//         return true;
//     }
    
//     printf("appid = %u, model_uuid = %u\n", appid, model_uuid);

//     count ++;
//     return false;
// }

// int main(int argc, char** argv)
// {
//     rx_activation_notification(&test_rx);
//     return 0;
// }