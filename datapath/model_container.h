#ifndef LITEFLOW_MODEL_CONTAINER
#define LITEFLOW_MODEL_CONTAINER

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include "types.h"

// Struct of models
// Model contains multiple layer
struct model_container;

extern int init_model(struct model_container *);

extern int destroy_model(struct model_container *);

extern int query_model(struct model_container *, s64 *input, s64 *output);

#endif