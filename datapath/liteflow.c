#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>

static int
__init liteflow_module_init(void)
{
    printk(KERN_INFO "LiteFlow init...\n");
    return 0;
}

static void
__exit liteflow_module_exit(void)
{
    printk(KERN_INFO "LiteFlow exit...\n");
}

module_init(liteflow_module_init);
module_exit(liteflow_module_exit);

MODULE_DESCRIPTION("LiteFlow");
MODULE_AUTHOR("Junxue ZHANG");
MODULE_LICENSE("GPL");