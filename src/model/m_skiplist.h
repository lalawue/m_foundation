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

#define SKT_KEY_MASK (~0)

typedef struct s_skt skt_t;

typedef struct s_skt_iter {
   void *prev;                  /* for internal */
   void *curr;                  /* for internal */
   uint64_t key;
   void *value;
} skt_iter_t;
   

skt_t* skt_create(void);
void skt_destroy(skt_t*);

int skt_count(skt_t*);

void* skt_query(skt_t*, uint64_t key);
void* skt_insert(skt_t*, uint64_t key, void *value);
void* skt_remove(skt_t*, uint64_t key);
   
void* skt_first(skt_t*);
void* skt_popf(skt_t*);

skt_iter_t* skt_iter_init(skt_t*, skt_iter_t*);
skt_iter_t* skt_iter_next(skt_t*, skt_iter_t*); /* use after iter_next */
void* skt_iter_remove(skt_t*, skt_iter_t*); /* remove current iter */

// use iter_remove within skt_foreach
#define skt_foreach(it, skt)                                            \
   for (skt_iter_t _##it, *it=skt_iter_init(skt, &_##it); (it=skt_iter_next(skt, it));)
   

#ifdef __cplusplus
}
#endif

#endif
