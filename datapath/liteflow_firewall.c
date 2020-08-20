// LiteFlow Firewall Kernel will register to both LiteFlow kernek and Kernel IPV4 Netfilter
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/version.h>

#include "linux/liteflow.h"
#include "liteflow_firewall.h"

static unsigned int
hook_func_incoming(void *priv,
		struct sk_buff *skb,
		const struct nf_hook_state *state)
{
    return NF_ACCEPT;
}

static unsigned int
hook_func_outcoming(void *priv,
	struct sk_buff *skb,
	const struct nf_hook_state *state)
{
    return NF_ACCEPT;
}

static struct nf_hook_ops
lf_firewall_ops_in __read_mostly = {
		hook: hook_func_incoming,
		hooknum: NF_INET_PRE_ROUTING,
		pf: PF_INET,
		priority: NF_IP_PRI_FIRST
};

static struct nf_hook_ops
lf_firewall_ops_out __read_mostly = {
		hook: hook_func_outcoming,
		hooknum: NF_INET_POST_ROUTING,
		pf: PF_INET,
		priority: NF_IP_PRI_FIRST
};

struct app lf_firewall_app = {
    .appid = LF_FIREWALL_APP_ID,
    .input_size = NUM_OF_INPUT_VALUE,
    .output_size = NUM_OF_OUTPUT_VALUE,
};

static int
__init liteflow_firewall_kernel_init(void)
{
	int ret;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
    nf_register_net_hook(&init_net, &lf_firewall_ops_in);
	nf_register_net_hook(&init_net, &lf_firewall_ops_out);
#else
    nf_register_hook(&lf_firewall_ops_in);
	nf_register_hook(&lf_firewall_ops_out);
#endif

	ret = lf_register_app(&lf_firewall_app);
    if (ret != LF_SUCCS) {
        printk(KERN_ERR "Cannot register liteflow firewall kernel with liteflow kernel!\n");
        return ret;
    }

	return ret;
}

static void
__exit liteflow_firewall_kernel_exit(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
	nf_unregister_net_hook(&init_net, &lf_firewall_ops_out);
    nf_unregister_net_hook(&init_net, &lf_firewall_ops_in);
#else
	nf_unregister_hook(&lf_firewall_ops_out);
	nf_unregister_hook(&lf_firewall_ops_in);
#endif
	
    lf_unregister_app(LF_FIREWALL_APP_ID);
}

module_init(liteflow_firewall_kernel_init);
module_exit(liteflow_firewall_kernel_exit);

MODULE_DESCRIPTION("liteflow firewall kernel");
MODULE_AUTHOR("Junxue ZHANG");
MODULE_LICENSE("GPL v2");
