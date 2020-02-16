#ifndef LITEFLOW_H
#define LITEFLOW_H

/* --- return code --- */
#define LF_SUCCS 0
#define LF_ERROR 4

/* --- data structure --- */
struct app {    
    u8 appid;
    u32 input_size;
    u32 output_size;
} __packed;

struct model_container {
    u32 uuid;
    u32 input_size;
    u32 output_size;
    struct list_head layers;
} __packed;

struct model_layer {
    u32 uuid;
    u32 input_size;
    u32 output_size;
    void (*comp_func) (s64 *input, s64 *output);
    struct list_head list; // layers are organized in a linked list

    // private part, will hide later use other techniques
    s64 *input;
    s64 *output;
} __packed;

/* --- public function --- */
int lf_register_app(struct app*);

int lf_unregister_app(u8 appid);

int lf_register_model(u8 appid, struct model_container *);

int lf_unregister_model(u8 appid, u32 model_uuid);

int lf_activate_model(u8 appid, u32 model_uuid);

int lf_query_model(u8 appid, s64 *input, s64 *output);

/* -- netlink related --- */
#define LF_NL_NAME "lf"
#define LF_NL_VERSION 1

#define LF_NL_MC_DEFAULT_NAME "default"

enum lf_multicast_groups {
    LF_NL_MC_DEFAULT, // Start from 0
    __LF_NL_MC_MAX,
};
#define LF_NL_MC_MAX (__LF_NL_MC_MAX - 1) 

enum lf_controls {
    LF_NL_C_UNSPEC,
    LF_NL_C_ACTIVATE_MODEL,
    LF_NL_C_REPORT_MODEL_ACTIVATION,
    __LF_NL_C_MAX,
};
#define LF_NL_C_MAX (__LF_NL_C_MAX - 1)

enum lf_attrs {
    LF_NL_ATTR_UNSPEC,
    LF_NL_ATTR_APP_ID,
    LF_NL_ATTR_MODEL_ID,
    __LF_NL_ATTR__MAX,
};
#define LF_NL_ATTR_MAX (__LF_NL_ATTR__MAX - 1)

#endif