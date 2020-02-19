#include "liblf.h"

#include "liteflow_nl.h"

static int rx_msg(struct nl_msg *msg, void* args)
{
    struct nlattr *attr[LF_NL_ATTR_MAX + 1];
    struct genlmsghdr *ghdr;
    int *ret_code = args;

    ghdr = genlmsg_hdr(nlmsg_hdr(msg));
    if(ghdr->cmd != LF_NL_C_ACTIVATE_MODEL_RET) {
        fprintf(stderr, "Not expected command...\n");
        return -1;
    }

    genlmsg_parse(nlmsg_hdr(msg), 0, attr, LF_NL_ATTR_MAX, lf_policy);

    if (!attr[LF_NL_ATTR_RET_CODE]) {
        fprintf(stderr, "Empty message RX!\n");
        return -1;
    }

    *ret_code = nla_get_u8(attr[LF_NL_ATTR_RET_CODE]);
    
    return 0;
}

int activate_model(__u8 appid, __u32 model_uuid)
{
    struct nl_sock* sock = NULL;
    struct nl_msg* msg = NULL;
    struct nl_cb *cb = NULL;
    int family_id;
    int ret;
    int ret_code;

    sock = connect_lf_genl_sock();
    if (sock == NULL) {
        return -1;
    }

    family_id = resolve_family_id(sock, LF_NL_NAME);
    if (family_id < 0) {
        return -1;
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

    ret = nla_put_u8(msg, LF_NL_ATTR_APP_ID, appid);
    if(ret < 0) {
        fprintf(stderr, "Unable to put app_id...\n");
        return -1;
    }

    ret = nla_put_u32(msg, LF_NL_ATTR_MODEL_ID, model_uuid);
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
    nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, rx_msg, &ret_code);
    
    ret = nl_recvmsgs(sock, cb);
    if (ret < 0) {
        fprintf(stderr, "failed to receive nl message!\n");
    }

    nl_cb_put(cb);
    nl_socket_free(sock);

    return ret_code;
}