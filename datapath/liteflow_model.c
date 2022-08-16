#include <linux/kernel.h>
#include <linux/slab.h>
#include "linux/liteflow.h"

#include "liteflow_model.h"

static inline int check_validate(struct model_container *model)
{
    struct model_layer *layer;

    u32 input_size = model->input_size;
    u32 output_size = model->output_size;
    u32 p_size = input_size;

    layer = list_first_entry_or_null(&model->layers, struct model_layer, list);
    // Check if layers are null
    if (layer == NULL) {
        printk(KERN_ERR "The layers are null.\n");
        return LF_ERROR;
    }
    // Check if the input size of first layer equals the input size of model
    if (layer->input_size != input_size) {
        printk(KERN_ERR "The input size of first layer is not equal to the input size of the model.\n");
        return LF_ERROR;
    }

    layer = list_last_entry(&model->layers, struct model_layer, list);
    if (layer->output_size != output_size) {
        printk(KERN_ERR "The output size of last layer is not equal to the output size of the model.\n");
        return LF_ERROR;
    }

    list_for_each_entry(layer, &model->layers, list) {
        if (layer ->comp_func == NULL) {
            printk(KERN_ERR "The computation function of layer with uuid: %u is NULL.\n", layer->uuid);
        }
        if (layer->input_size != p_size) {
            printk(KERN_ERR "The input size of layer with uuid: %u is not equal to its previous layer.\n", layer->uuid);
        }
        p_size = layer->output_size;
    }
    return LF_SUCCS;
}

int init_data_storage(struct model_container *model)
{
    struct model_layer *layer;
    u32 layer_input_size, layer_output_size;

    list_for_each_entry(layer, &model->layers, list) {
        layer_input_size = layer->input_size;
        layer_output_size = layer->output_size;

        layer->output = kmalloc(sizeof(s64) * layer_output_size, GFP_KERNEL);
        if (layer->output == NULL) {
            return LF_ERROR;
        }
    }
    return LF_SUCCS;
}

int init_model(struct model_container *model)
{
    u32 ret;
    ret = check_validate(model);
    if (ret == LF_ERROR) {
        return LF_ERROR;
    }
    ret = init_data_storage(model);
    if (ret == LF_ERROR) {
        return LF_ERROR;
    }
    return LF_SUCCS;
}

void destroy_model(struct model_container * model)
{
    struct model_layer *layer;

    list_for_each_entry(layer, &model->layers, list) {
        kfree(layer->input);
        kfree(layer->output);
    }
}

int query_model(struct model_container * model, s64 *input, s64 *output)
{
    struct model_layer *layer, *last_layer;
    s64 *intermediate;

    intermediate = input;
    list_for_each_entry(layer, &model->layers, list) {
        layer->comp_func(intermediate, layer->output);
        intermediate = layer->output;
    }

    last_layer = list_last_entry(&model->layers, struct model_layer, list);
    memcpy(output, last_layer->output, last_layer->output_size * sizeof(s64));

    return LF_SUCCS;
}

