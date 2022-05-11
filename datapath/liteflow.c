#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>

#include "linux/liteflow.h"

#include "liteflow_model.h"

#define MAX_APP 32
#define APP_ID_UNUSE 0

static DEFINE_RWLOCK(lf_lock);

struct __app {
    u8 appid;
    u32 input_size;
    u32 output_size;
    struct model_container *model_slot_0;
    struct model_container *model_slot_1;
    u8 active_model; // 0 or 1
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
    apps[appid].model_slot_0 = NULL;
    apps[appid].model_slot_1 = NULL;
    apps[appid].active_model = 0;
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
    model_to_delete = apps[appid].model_slot_0;
    if (model_to_delete!= NULL) {
        destroy_model(model_to_delete);
    }
    
    model_to_delete = apps[appid].model_slot_1;
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

    s8 info_model_uuid_0, info_model_uuid_1;

    if (appid > MAX_APP) {
        printk(KERN_ERR "Unsupported appid: %u.\n", appid);
        return LF_ERROR;
    }

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
    if (apps[appid].active_model == 0) {
        if (apps[appid].model_slot_1 != NULL) {
            printk(KERN_INFO "Deleting model witu uuid: %u to app with appid: %u...\n", apps[appid].model_slot_1->uuid, appid);
            destroy_model(apps[appid].model_slot_1);
        }
        apps[appid].model_slot_1 = model;
    } else {
        if (apps[appid].model_slot_0 != NULL) {
            printk(KERN_INFO "Deleting model witu uuid: %u to app with appid: %u...\n", apps[appid].model_slot_0->uuid, appid);
            destroy_model(apps[appid].model_slot_0);
        }
        apps[appid].model_slot_0 = model;
    }

    write_lock(&lf_lock);
    if (apps[appid].active_model == 0) {
        apps[appid].active_model = 1;
    } else {
        apps[appid].active_model = 0;
    }
    write_unlock(&lf_lock);
        
    printk(KERN_INFO "Model witu uuid: %u is registered to app with appid: %u!\n", model->uuid, appid);

    if (apps[appid].model_slot_0 != NULL) {
        info_model_uuid_0 = apps[appid].model_slot_0->uuid;
    } else {
        info_model_uuid_0 = -1;
    }

    if (apps[appid].model_slot_1 != NULL) {
        info_model_uuid_1 = apps[appid].model_slot_1->uuid;
    } else {
        info_model_uuid_1 = -1;
    }

    printk(KERN_INFO "Current slot 0 is registered with model: %d\n", info_model_uuid_0);
    printk(KERN_INFO "Current slot 1 is registered with model: %d\n", info_model_uuid_1);
    printk(KERN_INFO "Current active slot is: %u\n", apps[appid].active_model);

    return LF_SUCCS;

error:
    write_unlock(&lf_lock);
    return LF_ERROR;
}
EXPORT_SYMBOL(lf_register_model);

int 
lf_unregister_model(u8 appid, u32 model_uuid)
{
    struct model_container *model, *model_to_delete;
    u8 which_model;

    model_to_delete = NULL;

    if (appid > MAX_APP) {
        printk(KERN_ERR "Unsupported appid: %u.\n", appid);
        return LF_ERROR;
    }
        
    write_lock(&lf_lock);
    if (apps[appid].appid == APP_ID_UNUSE) {
        printk(KERN_ERR "Need to register app before unregistering model.\n");
        goto error;
    }

    model = apps[appid].model_slot_0;
    if (model != NULL && model->uuid == model_uuid) {
        model_to_delete = model;
        which_model = 0;
    }

    model = apps[appid].model_slot_1;
    if (model != NULL && model->uuid == model_uuid) {
        model_to_delete = model;
        which_model = 1;
    }

    printk(KERN_INFO "Unregister model with uuid: %u with app with appid: %u...\n", model_uuid, appid);

    if (model_to_delete == NULL) {
        printk(KERN_INFO "The model has already been deleted...\n");
        write_unlock(&lf_lock);

        return LF_SUCCS;
    }

    destroy_model(model_to_delete);
    if (which_model == 0) {
        apps[appid].model_slot_0 = NULL;
    } else {
        apps[appid].model_slot_1 = NULL;
    }
    write_unlock(&lf_lock);

    return LF_SUCCS;

error:
    write_unlock(&lf_lock);
    return LF_ERROR;
}
EXPORT_SYMBOL(lf_unregister_model);


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

    if (apps[appid].active_model == 0) {
        model_to_use = apps[appid].model_slot_0;
    } else {
        model_to_use = apps[appid].model_slot_1;
    }
    read_unlock(&lf_lock);

    if (model_to_use == NULL) {
        printk(KERN_ERR "No model for app with appid: %u.\n", appid);
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
        apps[appid].model_slot_0 = NULL;
        apps[appid].model_slot_1 = NULL;
        apps[appid].active_model = 0;
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
MODULE_LICENSE("GPL v2");