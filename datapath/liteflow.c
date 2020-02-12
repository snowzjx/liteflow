#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>

#include "linux/liteflow.h"

#include "model_container.h"

#define MAX_APP 32
#define APP_ID_UNUSE 0

static DEFINE_RWLOCK(lf_lock);

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
        printk(KERN_ERR "Unsupported appid: %u.\n", appid);
        return LF_ERROR;
    }

    write_lock(&lf_lock);
    if (apps[appid].appid != APP_ID_UNUSE) {
        printk(KERN_ERR "Cannot re-register app with appid: %u.\n", appid);
        goto error;
    }

    printk(KERN_INFO "Registering app with appid: %u...\n", app->appid);
    apps[appid].appid = appid;
    apps[appid].input_size = app->input_size;
    apps[appid].output_size = app->output_size;
    apps[appid].active_model = NULL;
    apps[appid].backup_model = NULL;
    write_unlock(&lf_lock);
    return LF_SUCCS;

error:
    write_unlock(&lf_lock);
    return LF_ERROR;
}
EXPORT_SYMBOL(lf_register_app);

int lf_unregister_app(u8 appid)
{
    struct model_container *model_to_delete;

    if (appid > MAX_APP) {
        printk(KERN_ERR "Unsupported appid: %u.\n", appid);
        return LF_ERROR;
    }

    write_lock(&lf_lock);

    if (apps[appid].appid == APP_ID_UNUSE) {
        printk(KERN_ERR "Cannot deregister app with unknown appid: %u.\n", appid);
        goto error;
    }

    printk(KERN_INFO "Unregistering app with appid: %u...\n", appid);
    model_to_delete = apps[appid].active_model;
    if (model_to_delete!= NULL) {
        destroy_model(model_to_delete);
    }
    
    model_to_delete = apps[appid].backup_model;
    if (model_to_delete!= NULL) {
        destroy_model(model_to_delete);
    }
    apps[appid].appid = APP_ID_UNUSE;

    write_unlock(&lf_lock);
    return LF_SUCCS;

error:
    write_unlock(&lf_lock);
    return LF_ERROR;
}
EXPORT_SYMBOL(lf_unregister_app);

int 
lf_register_model(u8 appid, struct model_container *model)
{
    u8 ret;
    if (appid > MAX_APP) {
        printk(KERN_ERR "Unsupported appid: %u.\n", appid);
        return LF_ERROR;
    }

    write_lock(&lf_lock);

    if (apps[appid].appid == APP_ID_UNUSE) {
        printk(KERN_ERR "Need to register app before registering model.\n");
        goto error;
    }

    if (apps[appid].input_size != model->input_size) {
        printk(KERN_ERR "Input size of app and model are not consistent.\n");
        goto error;
    }

    if (apps[appid].output_size != model->output_size) {
        printk(KERN_ERR "Output size of app and model are not consistent.\n");
        goto error;
    }

    printk(KERN_INFO "Registering model witu uuid: %u to app with appid: %u...\n", model->uuid, appid);
    ret = init_model(model);
    if (ret == LF_ERROR) {
        goto error;
    }
    apps[appid].backup_model = model;
    write_unlock(&lf_lock);
        
    printk(KERN_INFO "Model witu uuid: %u is registered to app with appid: %u!\n", model->uuid, appid);
    return LF_SUCCS;

error:
    write_unlock(&lf_lock);
    return LF_ERROR;
}
EXPORT_SYMBOL(lf_register_model);

int 
lf_unregister_model(u8 appid, u32 model_uuid)
{
    struct model_container *model;

    if (appid > MAX_APP) {
        printk(KERN_ERR "Unsupported appid: %u.\n", appid);
        return LF_ERROR;
    }
        
    write_lock(&lf_lock);
    if (apps[appid].appid == APP_ID_UNUSE) {
        printk(KERN_ERR "Need to register app before unregistering model.\n");
        goto error;
    }

    model = apps[appid].active_model;
    if (model != NULL && model->uuid == model_uuid) {
        printk(KERN_ERR "Cannot unregister active model.\n");
        goto error;
    }

    model = apps[appid].backup_model;
    if (model == NULL || model->uuid != model_uuid) {
        printk(KERN_ERR "No model with uuid: %u to unregister.\n", model_uuid);
        goto error;
    }
    printk(KERN_INFO "Unregister model with uuid: %u with app with appid: %u...\n", model_uuid, appid);

    destroy_model(model);
    apps[appid].backup_model = NULL;
    write_unlock(&lf_lock);

    return LF_SUCCS;

error:
    write_unlock(&lf_lock);
    return LF_ERROR;
}
EXPORT_SYMBOL(lf_unregister_model);

int 
lf_activate_model(u8 appid, u32 model_uuid) 
{
    struct model_container *model, *swap_model;

    if (appid > MAX_APP) {
        printk(KERN_ERR "Unsupported appid: %u.\n", appid);
        return LF_ERROR;
    }
        
    write_lock(&lf_lock);
    if (apps[appid].appid == APP_ID_UNUSE) {
        printk(KERN_ERR "Need to register app before unregistering model.\n");
        goto error;
    }

    model = apps[appid].backup_model;

    if (model == NULL) {
        printk(KERN_ERR "No backup model to activate.\n");
        goto error;
    } else if (model->uuid != model_uuid) {
        printk(KERN_ERR "First register the model with uuid: %u, then activate it.\n", model_uuid);
        goto error;
    }
    
    printk(KERN_INFO "Activate model with uuid: %u.\n", model_uuid);
    swap_model = apps[appid].active_model;
    apps[appid].active_model = model;
    apps[appid].backup_model = swap_model;

    // TODO swap some meta info, such as average reward of previous active model. which can be 
    // used as evidence to roll back model
    
    write_unlock(&lf_lock);
    return LF_SUCCS;

error:
    write_unlock(&lf_lock);
    return LF_ERROR;
}
EXPORT_SYMBOL(lf_activate_model);


int 
lf_query_model(u8 appid, s64 *input, s64 *output) {

    struct model_container *model_to_use;

    if (input == NULL || output == NULL) {
        printk(KERN_ERR "Input or output vector cannot be null.\n");
        return LF_ERROR;
    }


    if (appid > MAX_APP) {
        printk(KERN_ERR "Unsupported appid: %u.\n", appid);
        return LF_ERROR;
    }

    read_lock(&lf_lock);
    if (apps[appid].appid == APP_ID_UNUSE) {
        printk(KERN_ERR "Need to register app before inference the model.\n");
        write_unlock(&lf_lock);
        return LF_ERROR;
    }
    
    model_to_use = apps[appid].active_model;
    read_unlock(&lf_lock);

    if (model_to_use == NULL) {
        printk(KERN_ERR "No active model for app with appid: %u.\n", appid);
        return LF_ERROR;
    }

    printk(KERN_INFO "Using model with uuid: %u for inference...\n", model_to_use->uuid);

    return query_model(model_to_use, input, output);
}
EXPORT_SYMBOL(lf_query_model);

static int
__init liteflow_module_init(void)
{
    u8 appid;

    printk(KERN_INFO "liteflow init...\n");

    for (appid = 1; appid <= MAX_APP; ++appid) {
        apps[appid].appid = APP_ID_UNUSE;
        apps[appid].active_model = NULL;
        apps[appid].backup_model = NULL;
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