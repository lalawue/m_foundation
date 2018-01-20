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
   struct s_skt_node *forward[1]; /* variable sized array of forward pointers */
} skt_node_t;

struct s_skt {
   int randomsLeft;
   int randomBits;
   /* Maximum level of the list
      (1 more than the number of levels in the list) */
   int level;
   skt_node_t *header; /* pointer to header */
   skt_node_t *NIL;
   int count;
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
      
      lst->NIL = _newNodeOfLevel(0);
      lst->NIL->key = 0x7fffffff;
      
      lst->randomBits = prng_next(&lst->rng);
      lst->randomsLeft = kBitsInRandom / 2;
      
      lst->level = 0;
      lst->count = 0;
      
      lst->header = _newNodeOfLevel(kMaxNumberOfLevels);
      
      for (int i=0; i<kMaxNumberOfLevels; i++) {
         lst->header->forward[i] = lst->NIL;
      }
   }
   return lst;
}


void
skt_destroy(skt_t *lst, skt_callback cb) {
   if (lst) {
      skt_node_t *q, *p = lst->header;
      do {
         q = p->forward[0];
         if (cb && (p!=lst->NIL)) {
            cb(p->key, p->value);
         }
         mm_free(p);
         p = q;
      } while (p != lst->NIL);
      mm_free(lst->NIL);
      mm_free(lst);
   }
}

/* if key exist, return original value;
 * return new value when success
 * return NULL for other error
 */
void*
skt_insert(skt_t *lst, uint32_t key, void *value) {
   int k;
   skt_node_t *p,*q;
   skt_node_t *update[kMaxNumberOfLevels];   

   if (lst) {
      p = lst->header;
      k = lst->level;
      do {
         while (q = p->forward[k], q->key < key) {
            p = q;
         }
         update[k] = p;
      } while(--k >= 0);

      if (q->key == key) {
         return q->value;
      }

      k = _randomLevel(lst);
      if (k > lst->level) {
         k = ++lst->level;
         update[k] = lst->header;
      }
      q = _newNodeOfLevel(k);
      q->key = key;
      q->value = value;
      do {
         p = update[k];
         q->forward[k] = p->forward[k];
         p->forward[k] = q;
      } while(--k >= 0);

      lst->count += 1;
      return value;
   }
   return NULL;
}

void*
skt_remove(skt_t *lst, uint32_t key) {
   int k,m;
   skt_node_t *update[kMaxNumberOfLevels];
   skt_node_t *p, *q;
   void *ret_value = NULL;

   if (lst) {
      p = lst->header;
      k = m = lst->level;
      do {
         while (q = p->forward[k], q->key < key) {
            p = q;
         }
         update[k] = p;
      } while(--k >= 0);

      if (q->key == key) {
         for (k=0; k<=m && (p=update[k])->forward[k] == q; k++) {
            p->forward[k] = q->forward[k];
         }
         ret_value = q->value;
         mm_free(q);
         while (lst->header->forward[m]==lst->NIL && m>0) {
            m--;
         }
         lst->level = m;
         lst->count -= 1;
      }
   }
   return ret_value;
}

void*
skt_query(skt_t *lst, uint32_t key) {
   int k;
   skt_node_t *p, *q;
   
   if (lst) {
      p = lst->header;
      k = lst->level;
      do {
         while (q = p->forward[k], q->key < key) {
            p = q;
         }

         if (q->key == key) {
            return q->value;
         }         
      } while (--k >= 0);
   }
   return NULL;
}


void
skt_foreach(skt_t *lst, skt_callback cb) {
   if (lst && cb) {
      skt_node_t *q, *p = lst->header;
      do {
         q = p->forward[0];
         if (p!=lst->NIL && !cb(p->key, p->value)) {
            break;
         }
         p = q;
      } while (p != lst->NIL);      
   }
}

int
skt_count(skt_t *lst) {
   return lst ? lst->count : 0;
}
