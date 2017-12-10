/* 
 * Copyright (c) 2015 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef M_DICT_H
#define M_DICT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct s_dict dict_t;


/* helper
 */
typedef uint32_t(*dict_key_hash_callback)(const void *name, int len);

typedef void(*dict_enumerate_callback)(
   void *opaque, const void *key, int keylen, void *value, int *stop);

uint32_t dict_default_key_hash(const void *key, int keylen);




/* interface
 */

// default when capacity_init:0, expand_factor:0, cb:NULL
dict_t* dict_create(int capacity_init, float expand_factor, dict_key_hash_callback cb);
void dict_destroy(dict_t*, dict_enumerate_callback cb, void *opaque);

int dict_count(dict_t*);

void* dict_get(dict_t*, const void *key, int keylen);
int dict_set(dict_t*, const void *key, int keylen, void *value);

void* dict_remove(dict_t*, const void *key, int keylen);

void dict_foreach(dict_t*, dict_enumerate_callback cb, void *opaque);

#ifdef __cplusplus
}
#endif


#endif
