/* 
 * Copyright (c) 2015 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <assert.h>

#include "m_map.h"
#include "m_list.h"
#include "m_mem.h"

typedef struct s_map_kv {
   struct s_map_kv *next;
   lst_node_t *node;            /* node of list */
   uint32_t hash;
   void *value;
   int keylen;
   void *key;
} map_kv_t;

struct s_map {
   int count;
   int capacity;
   float factor;
   map_key_hash_callback hash_cb;
   lst_t *kv_lst;
   map_kv_t **kv_cache;
};

map_t*
map_create(int capacity_init, float expand_factor, map_key_hash_callback cb) {
   map_t *d = (map_t*)mm_malloc(sizeof(map_t));
   if (d) {
      d->capacity = capacity_init>0 ? capacity_init : 4;
      if (expand_factor>0 && expand_factor<1.0) {
         d->factor = expand_factor;
      } else {
         d->factor = 0.75;
      }
      d->hash_cb = cb ? cb : map_default_key_hash;
      d->kv_cache = (map_kv_t**)mm_malloc(capacity_init * sizeof(map_kv_t*));
      d->kv_lst = lst_create();
      return d;
   }
   return NULL;
}

void
map_destroy(map_t *d, map_enumerate_callback cb, void *opaque) {
   if (d) {
      while (lst_count(d->kv_lst)) {
         map_kv_t *kv = (map_kv_t*)lst_popf(d->kv_lst);
         if (cb) {
            cb(opaque, kv->key, kv->keylen, kv->value, NULL);
         }
         mm_free(kv);
      }
      lst_destroy(d->kv_lst);
      mm_free(d->kv_cache);
      mm_free(d);
   }
}

int
map_count(map_t *d) {
   if (d) {
      return d->count;
   }
   return -1;
}

uint32_t
map_default_key_hash(const void *key, int keylen) {
   uint32_t h = (uint32_t)keylen;
   for (int i=0; i<keylen; i++) {
      h = h ^ ((h<<5)+(h>>2)+(uint32_t)((unsigned char*)key)[i]);
   }
   return h;
}

static map_kv_t*
_map_get_kv(map_t *d, const void *key, int keylen, uint32_t *out_hash) {
   uint32_t hash = d->hash_cb(key, keylen);
   map_kv_t *kv = d->kv_cache[hash % d->capacity];

   if (out_hash) {
      *out_hash = hash;
   }

   while (kv) {
      if ((kv->hash==hash) && (kv->keylen==keylen) && (memcmp(kv->key, key, keylen)==0)) {
         return kv;
      }
      kv = kv->next;
   }
   return NULL;
}

static void
_map_update_index(map_t *d, map_kv_t *kv) {
   uint32_t h = kv->hash % d->capacity;
   kv->next = d->kv_cache[h];
   d->kv_cache[h] = kv;
}

static int
_map_expand(map_t *d) {
   if (d->count >= (int)(d->capacity * d->factor)) {
      int capacity = d->capacity << 1;
      map_kv_t **kv_cache = (map_kv_t**)mm_realloc(d->kv_cache, capacity * sizeof(map_kv_t*));
      if (kv_cache == NULL) {
         return 0;
      }

      d->kv_cache = kv_cache;
      d->capacity = capacity;

      memset(d->kv_cache, 0, capacity * sizeof(map_kv_t*));

      lst_foreach(it, d->kv_lst) {
         map_kv_t *kv = (map_kv_t*)lst_iter_data(it);
         _map_update_index(d, kv);
      }
   }
   return 1;         
}

void*
map_get(map_t *d, const void *key, int keylen) {
   if  (d && key && keylen>0) {
      map_kv_t *kv = _map_get_kv(d, key, keylen, NULL);
      if (kv) {
         return kv->value;
      }
   }
   return NULL;
}

/* return 1 when success stored
 */
int
map_set(map_t *d, const void *key, int keylen, void *value) {
   if (d && key && keylen>0 && value) {
      uint32_t hash = 0;
      map_kv_t *kv = _map_get_kv(d, key, keylen, &hash);

      if (kv) {
         kv->value = value;
         return 1;
      }

      if (_map_expand(d) <= 0) {
         return 0;
      }

      kv = (map_kv_t*)mm_malloc(sizeof(*kv) + keylen + 1);
      if (kv) {
         kv->node = lst_pushl(d->kv_lst, kv);
         kv->hash = hash;
         kv->value = value;
         kv->keylen = keylen;

         kv->key = (((unsigned char*)kv) + sizeof(*kv));
         memcpy(kv->key, key, keylen);

         _map_update_index(d, kv);
         d->count++;
         return 1;
      }
   }
   return 0;
}

void*
map_remove(map_t *d, const void *key, int keylen) {
   if (d && key && keylen) {
      map_kv_t *rkv = _map_get_kv(d, key, keylen, NULL);
      if (rkv) {
         void *value = rkv->value;
         uint32_t h = rkv->hash % d->capacity;
         map_kv_t *kv = d->kv_cache[h];

         if (kv == rkv) {
            d->kv_cache[h] = kv->next;
         } else {
            while (kv->next != rkv) {
               kv = kv->next;
            }
            kv->next = rkv->next;
         }

         lst_remove(d->kv_lst, rkv->node);
         mm_free(rkv);

         d->count--;
         return value;
      }
   }
   return NULL;
}

void
map_foreach(map_t *d, map_enumerate_callback cb, void *opaque) {
   if (d && cb) {
      int stop = 0;
      lst_foreach(it, d->kv_lst) {
         map_kv_t *e = (map_kv_t*)lst_iter_data(it);
         cb(opaque, e->key, e->keylen, e->value, &stop);
         if (stop) {
            break;
         }
      }
   }
}
