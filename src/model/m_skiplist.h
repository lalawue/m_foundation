/* 
 * Copyright (c) 2017 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef M_SKIPLIST_H
#define M_SKIPLIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct s_skt skt_t;
typedef void (*skt_finalize_callback)(int key, void *value);

skt_t* skt_create(void);
void skt_destroy(skt_t *lst, skt_finalize_callback cb);

void* skt_query(skt_t *lst, uint32_t key);
int   skt_insert(skt_t *lst, uint32_t key, void *value);
int   skt_remove(skt_t *lst, uint32_t key);

#ifdef __cplusplus
}
#endif

#endif
