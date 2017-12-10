/* 
 * Copyright (c) 2015 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef M_MAP_H
#define M_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct s_map map_t;


/* helper
 */
typedef uint32_t(*map_key_hash_callback)(const void *name, int len);

typedef void(*map_enumerate_callback)(
   void *opaque, const void *key, int keylen, void *value, int *stop);

uint32_t map_default_key_hash(const void *key, int keylen);




/* interface
 */

// default when capacity_init:0, expand_factor:0, cb:NULL
map_t* map_create(int capacity_init, float expand_factor, map_key_hash_callback cb);
void map_destroy(map_t*, map_enumerate_callback cb, void *opaque);

int map_count(map_t*);

void* map_get(map_t*, const void *key, int keylen);
int map_set(map_t*, const void *key, int keylen, void *value);

void* map_remove(map_t*, const void *key, int keylen);

void map_foreach(map_t*, map_enumerate_callback cb, void *opaque);

#ifdef __cplusplus
}
#endif


#endif
