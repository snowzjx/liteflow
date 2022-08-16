#include "liblf.h"

#include <stdbool.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>

#include "liteflow_nl.h"

struct ops_wrapper {
    bool (*ops)(__s64 *data, __u32 size);
    bool shoud_stop;
};

static int rx_msg(struct nl_msg *msg, void* args)
{
    struct nlattr *attr[LF_NL_ATTR_MAX + 1];
    struct ops_wrapper *wrapper = args;
    __s64 *data;
    __u32 length;

    genlmsg_parse(nlmsg_hdr(msg), 0, attr, LF_NL_ATTR_MAX, lf_policy);

    if (!attr[LF_NL_ATTR_SIZE]) {
        fprintf(stderr, "Kernel sent empty message!\n");
        return NL_OK;
    }

    length = nla_get_u32(attr[LF_NL_ATTR_SIZE]);

    if (!attr[LF_NL_ATTR_DATA]) {
        fprintf(stderr, "Kernel sent empty message!\n");
        return NL_OK;
    }

    data = nla_data(attr[LF_NL_ATTR_DATA]);
    wrapper->shoud_stop = wrapper->ops(data, length);

    return 0;
}

int rx_dp_notification(bool (*rx)(__s64 *data, __u32 length))
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