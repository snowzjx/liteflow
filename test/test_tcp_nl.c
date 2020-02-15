#include <stdio.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>

#include "linux/liteflow_tcp.h"

static int skip_seq_check(struct nl_msg *msg, void *arg)
{
	return NL_OK;
}

static int print_report(struct nl_msg *msg, void* arg)
{
	struct nlattr *attr[LF_TCP_NL_ATTR_MAX + 1];
	int pos;

	genlmsg_parse(nlmsg_hdr(msg), 0, attr, LF_TCP_NL_ATTR_MAX, lf_tcp_policy);

	if (!attr[LF_TCP_NL_ATTR_NN_INPUT]) {
		fprintf(stderr, "Kernel sent empty message!\n");
		return NL_OK;
	}

	__s64 *nn_input = nla_data(attr[LF_TCP_NL_ATTR_NN_INPUT]);

	for (pos = 0; pos < NUM_OF_GLOBAL_STATS + NUM_OF_INPUT_METRICS * HISTORY_LEN; ++pos) {
		fprintf(stdout, "%lld\t", nn_input[pos]);
	}
	fprintf(stdout, "\n");

	return NL_OK;
}

int main(int argc, char** argv)
{
	struct nl_sock* sock = NULL;
    struct nl_cb *cb = NULL;
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

    family_id = genl_ctrl_resolve(sock, LF_TCP_NL_NAME);
	if(family_id < 0) {
		fprintf(stderr, "Unable to resolve family name...\n");
		return -1;
	}
    fprintf(stdout, "Returned family id is %d...\n", family_id);

	grp_id = genl_ctrl_resolve_grp(sock, LF_TCP_NL_NAME, LF_TCP_NL_MC_DEFAULT_NAME);
	if (grp_id < 0) {
		fprintf(stderr, "Unable to resolve group name...\n");
		return -1;
	}
    fprintf(stdout, "Returned group id is %d...\n", grp_id);

	if (nl_socket_add_membership(sock, grp_id)) {
		fprintf(stderr, "Unable to join group %u!\n", grp_id); 
	}

    cb = nl_cb_alloc(NL_CB_DEFAULT);
	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, skip_seq_check, NULL);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, print_report, NULL);
	do {
		ret = nl_recvmsgs(sock, cb);
	} while (!ret);

	nl_cb_put(cb);
    nl_socket_free(sock);

	return 0;
}