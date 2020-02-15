#ifndef LITEFLOW_MODEL_H
#define LITEFLOW_MODEL_H

struct model_container;

extern int init_model(struct model_container *);

extern void destroy_model(struct model_container *);

extern int query_model(struct model_container *, s64 *input, s64 *output);

#endif