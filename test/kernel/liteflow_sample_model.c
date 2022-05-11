/****************** main.c for model 2333 generated by nn-loader.py *******************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/slab.h>

#include "linux/liteflow.h"


#include "linux/tanh_lookup_table.h"



/************************************** Layer 0 **************************************/
static void quan_0_comp (s64 *input, s64 *output)
{
    // Q_min: 0.0
    // Q_max: 62.0
    
    output[0] = (input[0] - 0) * 255 / 62 + -128;
      
    printk(KERN_INFO "Output_quan_0_%d: %lld", 0, output[0]);
    
    
    output[1] = (input[1] - 0) * 255 / 62 + -128;
      
    printk(KERN_INFO "Output_quan_0_%d: %lld", 1, output[1]);
    
    
    output[2] = (input[2] - 0) * 255 / 62 + -128;
      
    printk(KERN_INFO "Output_quan_0_%d: %lld", 2, output[2]);
    
    
    output[3] = (input[3] - 0) * 255 / 62 + -128;
      
    printk(KERN_INFO "Output_quan_0_%d: %lld", 3, output[3]);
    
    
}

struct model_layer layer_0 __read_mostly = {
    .uuid = 0,
    .input_size = 4,
    .output_size = 4,
    .comp_func = quan_0_comp,
};
/************************************ End Layer 0 ************************************/

/************************************** Layer 1 **************************************/
static void fc_1_comp (s64 *input, s64 *output)
{
    
    output[0] =(input[0] + 128) * (127 + 0) + (input[1] + 128) * (-83 + 0) + (input[2] + 128) * (16 + 0) + (input[3] + 128) * (-10 + 0) + (-458);
    output[0] = ((output[0] * 7361 / 10000)  >> -(-8)) + 4;
    
    printk(KERN_INFO "Output_fc_1_%d: %lld", 0, output[0]);
    
    
    output[1] =(input[0] + 128) * (-97 + 0) + (input[1] + 128) * (58 + 0) + (input[2] + 128) * (82 + 0) + (input[3] + 128) * (4 + 0) + (0);
    output[1] = ((output[1] * 7361 / 10000)  >> -(-8)) + 4;
    
    printk(KERN_INFO "Output_fc_1_%d: %lld", 1, output[1]);
    
    
    output[2] =(input[0] + 128) * (63 + 0) + (input[1] + 128) * (-52 + 0) + (input[2] + 128) * (0 + 0) + (input[3] + 128) * (67 + 0) + (0);
    output[2] = ((output[2] * 7361 / 10000)  >> -(-8)) + 4;
    
    printk(KERN_INFO "Output_fc_1_%d: %lld", 2, output[2]);
    
    
    output[3] =(input[0] + 128) * (-84 + 0) + (input[1] + 128) * (-8 + 0) + (input[2] + 128) * (0 + 0) + (input[3] + 128) * (-94 + 0) + (0);
    output[3] = ((output[3] * 7361 / 10000)  >> -(-8)) + 4;
    
    printk(KERN_INFO "Output_fc_1_%d: %lld", 3, output[3]);
    
    
    output[4] =(input[0] + 128) * (-50 + 0) + (input[1] + 128) * (-100 + 0) + (input[2] + 128) * (-30 + 0) + (input[3] + 128) * (-39 + 0) + (0);
    output[4] = ((output[4] * 7361 / 10000)  >> -(-8)) + 4;
    
    printk(KERN_INFO "Output_fc_1_%d: %lld", 4, output[4]);
    
    
    output[5] =(input[0] + 128) * (10 + 0) + (input[1] + 128) * (83 + 0) + (input[2] + 128) * (63 + 0) + (input[3] + 128) * (35 + 0) + (0);
    output[5] = ((output[5] * 7361 / 10000)  >> -(-8)) + 4;
    
    printk(KERN_INFO "Output_fc_1_%d: %lld", 5, output[5]);
    
    
    output[6] =(input[0] + 128) * (-13 + 0) + (input[1] + 128) * (14 + 0) + (input[2] + 128) * (61 + 0) + (input[3] + 128) * (-5 + 0) + (0);
    output[6] = ((output[6] * 7361 / 10000)  >> -(-8)) + 4;
    
    printk(KERN_INFO "Output_fc_1_%d: %lld", 6, output[6]);
    
    
    output[7] =(input[0] + 128) * (15 + 0) + (input[1] + 128) * (54 + 0) + (input[2] + 128) * (30 + 0) + (input[3] + 128) * (-63 + 0) + (-374);
    output[7] = ((output[7] * 7361 / 10000)  >> -(-8)) + 4;
    
    printk(KERN_INFO "Output_fc_1_%d: %lld", 7, output[7]);
    
    
}

struct model_layer layer_1 __read_mostly = {
    .uuid = 1,
    .input_size = 4,
    .output_size = 8,
    .comp_func = fc_1_comp,
};
/************************************ End Layer 1 ************************************/

/************************************** Layer 2 **************************************/
static void tanh_2_comp (s64 *input, s64 *output)
{
    // Input Scale: 5922 / 10000
    // Output Scale: 78 / 10000
    // Input Offset: -4
    // Output Offset: 0
    
    output[0] = lf_tanh((input[0] + -4) * TANH_X_SCALE_UP * 5922 / 10000) * 10000 / 78 / TANH_Y_SCALE_DOWN;    
      
    printk(KERN_INFO "Output_tanh_2_%d: %lld", 0, output[0]);
    
    
    output[1] = lf_tanh((input[1] + -4) * TANH_X_SCALE_UP * 5922 / 10000) * 10000 / 78 / TANH_Y_SCALE_DOWN;    
      
    printk(KERN_INFO "Output_tanh_2_%d: %lld", 1, output[1]);
    
    
    output[2] = lf_tanh((input[2] + -4) * TANH_X_SCALE_UP * 5922 / 10000) * 10000 / 78 / TANH_Y_SCALE_DOWN;    
      
    printk(KERN_INFO "Output_tanh_2_%d: %lld", 2, output[2]);
    
    
    output[3] = lf_tanh((input[3] + -4) * TANH_X_SCALE_UP * 5922 / 10000) * 10000 / 78 / TANH_Y_SCALE_DOWN;    
      
    printk(KERN_INFO "Output_tanh_2_%d: %lld", 3, output[3]);
    
    
    output[4] = lf_tanh((input[4] + -4) * TANH_X_SCALE_UP * 5922 / 10000) * 10000 / 78 / TANH_Y_SCALE_DOWN;    
      
    printk(KERN_INFO "Output_tanh_2_%d: %lld", 4, output[4]);
    
    
    output[5] = lf_tanh((input[5] + -4) * TANH_X_SCALE_UP * 5922 / 10000) * 10000 / 78 / TANH_Y_SCALE_DOWN;    
      
    printk(KERN_INFO "Output_tanh_2_%d: %lld", 5, output[5]);
    
    
    output[6] = lf_tanh((input[6] + -4) * TANH_X_SCALE_UP * 5922 / 10000) * 10000 / 78 / TANH_Y_SCALE_DOWN;    
      
    printk(KERN_INFO "Output_tanh_2_%d: %lld", 6, output[6]);
    
    
    output[7] = lf_tanh((input[7] + -4) * TANH_X_SCALE_UP * 5922 / 10000) * 10000 / 78 / TANH_Y_SCALE_DOWN;    
      
    printk(KERN_INFO "Output_tanh_2_%d: %lld", 7, output[7]);
    
    
}

struct model_layer layer_2 __read_mostly = {
    .uuid = 2,
    .input_size = 8,
    .output_size = 8,
    .comp_func = tanh_2_comp,
};
/************************************ End Layer 2 ************************************/

/************************************** Layer 3 **************************************/
static void fc_3_comp (s64 *input, s64 *output)
{
    
    output[0] =(input[0] + 0) * (127 + 0) + (input[1] + 0) * (73 + 0) + (input[2] + 0) * (65 + 0) + (input[3] + 0) * (-54 + 0) + (input[4] + 0) * (-44 + 0) + (input[5] + 0) * (57 + 0) + (input[6] + 0) * (54 + 0) + (input[7] + 0) * (75 + 0) + (7469);
    output[0] = ((output[0] * 8388 / 10000)  >> -(-8)) + -128;
    
    printk(KERN_INFO "Output_fc_3_%d: %lld", 0, output[0]);
    
    
}

struct model_layer layer_3 __read_mostly = {
    .uuid = 3,
    .input_size = 8,
    .output_size = 1,
    .comp_func = fc_3_comp,
};
/************************************ End Layer 3 ************************************/

/************************************** Layer 4 **************************************/
static void dequan_4_comp (s64 *input, s64 *output)
{
    // Q_min: 0.0
    // Q_max: 29.982952117919922
    
    output[0] = (input[0] + 128) * 29 / 255 + 0;
    
}

struct model_layer layer_4 __read_mostly = {
    .uuid = 4,
    .input_size = 1,
    .output_size = 1,
    .comp_func = dequan_4_comp,
};
/************************************ End Layer 4 ************************************/


/************************************** Model  **************************************/

struct model_container model_2333 __read_mostly = {
    .uuid = 2333,
    .input_size = 4,
    .output_size = 1,
};


/******************************************** Test Mode ********************************************/
struct app app = {
    .appid = 1,
    .input_size = 4,
    .output_size = 1,
};

/**************************************** End Test Mode ********************************************/


static int
__init liteflow_2333_model_init(void)
{
    
    s64 _input[4];
    s64 _output[1];
    int _output_pos;
    

    // Construct layers
    INIT_LIST_HEAD(&model_2333.layers);
    list_add(&layer_0.list, &model_2333.layers);
    list_add(&layer_1.list, &layer_0.list);
    list_add(&layer_2.list, &layer_1.list);
    list_add(&layer_3.list, &layer_2.list);
    list_add(&layer_4.list, &layer_3.list);

    
    // Test mode = on
    lf_register_app(&app);
    
    
    lf_register_model(1, &model_2333);
    
    // TODO
    _input[0] = 41 ;
    _input[1] = 50 ;
    _input[2] = 60 ;
    _input[3] = 60 ;

    lf_query_model(1, _input, _output);

    for (_output_pos = 0; _output_pos < 1; ++_output_pos) {
        printk(KERN_INFO "Output_%d: %lld\n", _output_pos, _output[_output_pos]);
    }
    

    return 0;
}

static void
__exit liteflow_2333_model_exit(void)
{
    lf_unregister_model(1, 2333);

    
    // Test mode = on
    lf_unregister_app(1);
    
}

module_init(liteflow_2333_model_init);
module_exit(liteflow_2333_model_exit);

MODULE_DESCRIPTION("liteflow 2333 model");
MODULE_AUTHOR("liteflow");
MODULE_LICENSE("GPL v2");

/************************************ End Model  ************************************/