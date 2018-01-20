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

/* return 0 to stop, ignore in destroy */   
typedef int (*skt_callback)(int key, void *value);
   

skt_t* skt_create(void);
void skt_destroy(skt_t *lst, skt_callback cb);
   

void* skt_query(skt_t *lst, uint32_t key);
void* skt_insert(skt_t *lst, uint32_t key, void *value);
void* skt_remove(skt_t *lst, uint32_t key);
   

void skt_foreach(skt_t *lst, skt_callback cb);
   
int skt_count(skt_t *lst);

#ifdef __cplusplus
}
#endif

#endif
