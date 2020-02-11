#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>

#include "linux/liteflow.h"

#include "model_container.h"

#define MAX_APP 32
#define APP_ID_UNUSE 0

static DEFINE_RWLOCK(app_lock);

struct __app {
    u8 appid;
    u32 input_size;
    u32 output_size;
    struct model_container *active_model;
    struct model_container *backup_model;
    struct hlist_node node;
    // other metrics information
};

static struct __app apps[MAX_APP + 1];


int lf_register_app(struct app* app)
{
    u8 appid;

    appid = app->appid;
    if (appid > MAX_APP) {
        printk(KERN_ERR "unsupported appid: %u\n", appid);
        return LF_ERROR;
    }

    if (apps[appid].appid != APP_ID_UNUSE) {
        printk(KERN_ERR "cannot re-register app with appid: %u\n", appid);
        return LF_ERROR;
    }

    write_lock(&app_lock);
    printk(KERN_INFO "registering app with appid: %u\n", app->appid);
    apps[appid].appid = appid;
    apps[appid].input_size = app->input_size;
    apps[appid].output_size = app->output_size;
    apps[appid].active_model = NULL;
    apps[appid].backup_model = NULL;
    write_unlock(&app_lock);
    return LF_SUCCS;
}
EXPORT_SYMBOL(lf_register_app);

int lf_unregister_app(u8 appid)
{
    struct model_container *model_to_delete;

    if (appid > MAX_APP) {
        printk(KERN_ERR "unsupported appid: %u\n", appid);
        return LF_ERROR;
    }

    if (apps[appid].appid == APP_ID_UNUSE) {
        printk(KERN_ERR "cannot deregister app with unknown appid: %u\n", appid);
        return LF_ERROR;
    }

    write_lock(&app_lock);
    printk(KERN_INFO "unregistering app with appid: %u\n", appid);
    model_to_delete = apps[appid].active_model;
    if (model_to_delete!= NULL) {
        destroy_model(model_to_delete);
    }
    
    model_to_delete = apps[appid].backup_model;
    if (model_to_delete!= NULL) {
        destroy_model(model_to_delete);
    }
    apps[appid].appid = APP_ID_UNUSE;

    write_unlock(&app_lock);
    return LF_ERROR;
}
EXPORT_SYMBOL(lf_unregister_app);

int 
lf_register_model(u8 appid, struct model_container *model)
{
    u32 ret;

    ret = init_model(model);
    if (ret == LF_ERROR) {
        return LF_ERROR;
    }

    return LF_SUCCS;
}
EXPORT_SYMBOL(lf_register_model);

int 
lf_unregister_model(u8 appid, struct model_container *model)
{
    return LF_SUCCS;
}
EXPORT_SYMBOL(lf_unregister_model);

static int
__init liteflow_module_init(void)
{
    u8 appid;

    printk(KERN_INFO "liteflow init...\n");

    for (appid = 1; appid <= MAX_APP; ++appid) {
        apps[appid].appid = APP_ID_UNUSE;
    }

    return 0;
}

static void
__exit liteflow_module_exit(void)
{
    printk(KERN_INFO "liteflow exit...\n");
}

module_init(liteflow_module_init);
module_exit(liteflow_module_exit);

MODULE_DESCRIPTION("liteflow");
MODULE_AUTHOR("Junxue ZHANG");
MODULE_LICENSE("GPL");