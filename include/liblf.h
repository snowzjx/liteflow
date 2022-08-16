#ifndef LIB_LF_H
#define LIB_LF_H

#include <stdbool.h>
#include <linux/kernel.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>

// The rx function is a customized function
// The return denotes to 'should_stop'
extern int rx_dp_notification(bool (*rx)(__s64 *data, __u32 length));

// --- Tools ---
static struct nl_sock* connect_lf_genl_sock()
{
    struct nl_sock* sock;

    sock = nl_socket_alloc();
    if (sock == NULL) {
        fprintf(stderr, "Cannot allocate sock...\n");
        return NULL;
    }

    nl_socket_disable_seq_check(sock);
    nl_socket_disable_auto_ack(sock);

    if (genl_connect(sock)) {
        fprintf(stderr, "Unable to connect to genl...\n");
        return NULL;
    }

    return sock;
}

static int resolve_family_id(struct nl_sock* sock, char* family_name)
{
    int family_id;

    family_id = genl_ctrl_resolve(sock, family_name);
    if(family_id < 0) {
        fprintf(stderr, "Unable to resolve family name...\n");
        return -1;
    }
    fprintf(stdout, "Returned family id is %d...\n", family_id);
    return family_id;
}

static int resolve_grp_id(struct nl_sock* sock, char* family_name, char* group_name)
{
    int grp_id;

    grp_id = genl_ctrl_resolve_grp(sock, family_name, group_name);
    if (grp_id < 0) {
        fprintf(stderr, "Unable to resolve group name...\n");
        return -1;
    }
    fprintf(stdout, "Returned group id is %d...\n", grp_id);

    return grp_id;
}

#endif