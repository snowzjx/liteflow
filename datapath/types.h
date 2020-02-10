#ifndef LITEFLOW_DATATYPES
#define LITEFLOW_DATATYPES

#define LF_ERROR -1
#define LF_SUCCS 0

struct model_container {
    u32 uuid;
    u32 input_size;
    u32 output_size;
    struct list_head layers;
} __packed;

struct model_layer {
    struct list_head list; // layers are organized in a linked list
    u32 uuid;
    u32 input_size;
    u32 output_size;
    void (*comp_func) (s64 *input, s64 *output);

    // private part
    s64 *input;
    s64 *output;
} __packed;

#endif