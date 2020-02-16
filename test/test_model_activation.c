#include <stdio.h>
#include <stdlib.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/mngt.h>

#include "liteflow_nl.h"

#define APP_ID 31
#define MODEL_UUID 2333

static int rx_reply(struct nl_msg *msg, void* args)
{
    fprintf(stdout, "%s called...\n", __FUNCTION__);
    return NL_OK;
}

static int rx_multicast(struct nl_msg *msg, void* args)
{
    fprintf(stdout, "%s called...\n", __FUNCTION__);
    return NL_OK;
}


static int skip_seq_check(struct nl_msg *msg, void *arg)
{
    return NL_OK;
}

static int rx_msg(struct nl_msg *msg, void* args)
{
    struct genlmsghdr *ghdr = genlmsg_hdr(nlmsg_hdr(msg));
    if(ghdr->cmd == LF_NL_C_ACTIVATE_MODEL_RET) {
        return rx_reply(msg, args);
    } else if(ghdr->cmd == LF_NL_C_REPORT_MODEL_ACTIVATION){
        return rx_multicast(msg, args);
    }
    return -1;
}

int main(int argc, char** argv)
{
    struct nl_sock* sock = NULL;
    struct nl_cb *cb = NULL;
    struct nl_msg* msg;
    int ret, family_id, grp_id;

    sock = nl_socket_alloc();
    if (sock == NULL) {
        fprintf(stderr, "Cannot allocate sock...\n");
        return -1;
    }

    nl_socket_disable_seq_check(sock);
    nl_socket_disable_auto_ack(sock);

    /* connect to genl */
    if (genl_connect(sock)) {
        fprintf(stderr, "Unable to connect to genl...\n");
        return -1;
    }

    family_id = genl_ctrl_resolve(sock, LF_NL_NAME);
    if(family_id < 0) {
        fprintf(stderr, "Unable to resolve family name...\n");
        return -1;
    }
    fprintf(stdout, "Returned family id is %d...\n", family_id);

    grp_id = genl_ctrl_resolve_grp(sock, LF_NL_NAME, LF_NL_MC_DEFAULT_NAME);
    if (grp_id < 0) {
        fprintf(stderr, "Unable to resolve group name...\n");
        return -1;
    }
    fprintf(stdout, "Returned group id is %d...\n", grp_id);

    if (nl_socket_add_membership(sock, grp_id)) {
        fprintf(stderr, "Unable to join group %u!\n", grp_id); 
    }

    msg = nlmsg_alloc();
    if(msg == NULL) {
        fprintf(stderr, "Unable to allocate msg...\n");
        return -1;
    }

    if(!genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, family_id, 0, 
        NLM_F_REQUEST, LF_NL_C_ACTIVATE_MODEL, 0)) {
        fprintf(stderr, "Unable to put header into msg...\n");
        return -1;
    }

    ret = nla_put_u8(msg, LF_NL_ATTR_APP_ID, APP_ID);
    if(ret < 0) {
        fprintf(stderr, "Unable to put app_id...\n");
        return -1;
    }

    ret = nla_put_u32(msg, LF_NL_ATTR_MODEL_ID, MODEL_UUID);
    if(ret < 0) {
        fprintf(stderr, "Unable to put model_uuid...\n");
        return -1;
    }

    ret = nl_send_auto(sock, msg);
    if (ret < 0) {
        fprintf(stderr, "failed to send nl message!\n");
    }

    nlmsg_free(msg);

    cb = nl_cb_alloc(NL_CB_DEFAULT);
    nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, skip_seq_check, NULL);
    nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, rx_msg, NULL);
    do {
        ret = nl_recvmsgs(sock, cb);
    } while (!ret);

    return 0;
}