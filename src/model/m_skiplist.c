// 
// 
// Copyright (c) 2017 lalawue
// 
// This library is free software; you can redistribute it and/or modify it
// under the terms of the MIT license. See LICENSE for details.
// 
// 

/* skiplist from ftp://ftp.cs.umd.edu/pub/skipLists/skipLists.c
 */

#include <time.h>
#include <stdlib.h>

#include "m_mem.h"
#include "m_skiplist.h"
#include "m_prng.h"

#define kBitsInRandom 31
#define kMaxNumberOfLevels 16
#define kMaxLevel (kMaxNumberOfLevels - 1)

typedef struct s_skt_node {
   uint32_t key;
   void *value;
   struct s_skt_node *forward[1]; /* forward pointers */
} skt_node_t;

struct s_skt {
   int randomsLeft;
   int randomBits;
   /* Maximum level of the list
      (1 more than the number of levels in the list) */
   int level;
   int count;   
   skt_node_t *header; /* pointer to header */
   skt_node_t *tail;   /* pointer to tail */
   prng_t rng;
};

static inline skt_node_t*
_newNodeOfLevel(int level) {
   return (skt_node_t*)mm_malloc(sizeof(skt_node_t) + level*sizeof(skt_node_t*));
}

static inline int
_randomLevel(skt_t *lst) {
   int level = 0;
   int b;
   do {
      b = lst->randomBits & 3;  /* 25% */
      if (!b) {
         level++;
      }
      lst->randomBits>>=2;
      if (--lst->randomsLeft == 0) {
         lst->randomBits = prng_next(&lst->rng);
         lst->randomsLeft = kBitsInRandom/2;
      }
   } while (!b);
   return (level > kMaxLevel ? kMaxLevel : level);
}

skt_t*
skt_create(void) {
   skt_t *lst = (skt_t*)mm_malloc(sizeof(*lst));
   if (lst) {
      prng_init(&lst->rng);
      
      lst->tail = _newNodeOfLevel(0);
      lst->tail->key = 0x7fffffff;
      
      lst->randomBits = prng_next(&lst->rng);
      lst->randomsLeft = kBitsInRandom / 2;
      
      lst->level = 0;
      lst->count = 0;
      
      lst->header = _newNodeOfLevel(kMaxNumberOfLevels);
      
      for (int i=0; i<kMaxNumberOfLevels; i++) {
         lst->header->forward[i] = lst->tail;
      }
   }
   return lst;
}


void
skt_destroy(skt_t *lst) {
   if (lst) {
      skt_node_t *next, *p = lst->header->forward[0];
      while (p != lst->tail) {
         next = p->forward[0];
         mm_free(p);
         p = next;
      }
      mm_free(lst->header);
      mm_free(lst->tail);
      mm_free(lst);
   }
}


int
skt_count(skt_t *lst) {
   return lst ? lst->count : 0;
}

void*
skt_query(skt_t *lst, uint32_t key) {
   int k;
   skt_node_t *p, *next;
   
   if (lst) {
      p = lst->header;
      k = lst->level;
      do {
         while (next = p->forward[k], next->key < key) {
            p = next;
         }

         if (next->key == key) {
            return next->value;
         }         
      } while (--k >= 0);
   }
   return NULL;
}

   
/* if key exist, return original value
 * return new value when success
 * return NULL for other error
 */
void*
skt_insert(skt_t *lst, uint32_t key, void *value) {
   if (lst) {
      int k;
      skt_node_t *p, *next;
      skt_node_t *update[kMaxNumberOfLevels];   
      
      p = lst->header;
      k = lst->level;
      do {
         while (next = p->forward[k], next->key < key) {
            p = next;
         }
         update[k] = p;
      } while(--k >= 0);

      if (next->key == key) {
         return next->value;
      }

      k = _randomLevel(lst);
      if (k > lst->level) {
         k = ++lst->level;
         update[k] = lst->header;
      }
      next = _newNodeOfLevel(k);
      next->key = key;
      next->value = value;
      do {
         p = update[k];
         next->forward[k] = p->forward[k];
         p->forward[k] = next;
      } while(--k >= 0);
      lst->count += 1;
      return value;
   }
   return NULL;
}

void*
skt_remove(skt_t *lst, uint32_t key) {
   void *ret_value = NULL;
   if (lst) {
      int k, m;
      skt_node_t *p, *next;
      skt_node_t *update[kMaxNumberOfLevels];      
      
      p = lst->header;
      k = m = lst->level;
      do {
         while (next = p->forward[k], next->key < key) {
            p = next;
         }
         update[k] = p;
      } while(--k >= 0);

      if (next->key == key) {
         ret_value = next->value;         
         for (k=0; k<=m && (p=update[k])->forward[k] == next; k++) {
            p->forward[k] = next->forward[k];
         }
         mm_free(next);
         while (lst->header->forward[m]==lst->tail && m>0) {
            m--;
         }
         lst->level = m;
         lst->count -= 1;
      }
   }
   return ret_value;
}

void*
skt_first(skt_t *lst) {
   if (lst && lst->count>0) {
      return lst->header->forward[0]->value;
   }
   return NULL;
}

void*
skt_popf(skt_t *lst) {
   if (lst && lst->count>0) {
      skt_node_t *p = lst->header->forward[0];
      return skt_remove(lst, p->key);
   }
   return NULL;
}


skt_iter_t*
skt_iter_init(skt_t *lst, skt_iter_t *it) {
   if (lst && lst->count>0 && it) {
      it->opaque = NULL;
      it->key = 0;
      it->value = NULL;
      return it;
   }
   return NULL;
}

skt_iter_t*
skt_iter_next(skt_t *lst, skt_iter_t *it) {
   if (lst && lst->count>0 && it) {
      skt_node_t *prev = (skt_node_t*)it->opaque;
      if (prev == NULL) {
         it->opaque = lst->header;         
      }
      else {
         it->opaque = prev->forward[0];
      }
      skt_node_t *p = ((skt_node_t*)it->opaque)->forward[0];   
      it->key = p->key;
      it->value = p->value;
      if (p != lst->tail) {
         return it;
      }
   }
   return NULL;
}
