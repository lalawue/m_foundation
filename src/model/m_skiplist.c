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

#define BitsInRandom 31
#define MaxNumberOfLevels 16
#define MaxLevel (MaxNumberOfLevels - 1)

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
};

static inline skt_node_t*
_newNodeOfLevel(int level) {
   return mm_malloc(sizeof(skt_node_t) + level*sizeof(skt_node_t*));
}

static void
_skt_init(skt_t *lst) { 
   lst->NIL = _newNodeOfLevel(0);
   lst->NIL->key = 0x7fffffff;
   lst->randomBits = random();
   lst->randomsLeft = BitsInRandom/2;
};

skt_t*
skt_create(void) {

   skt_t *lst = (skt_t*)mm_malloc(sizeof(*lst));
   if (lst) {
      _skt_init(lst);
      lst->level = 0;
      lst->header = _newNodeOfLevel(MaxNumberOfLevels);
      for (int i=0; i<MaxNumberOfLevels; i++) {
         lst->header->forward[i] = lst->NIL;
      }
   }
   return lst;
}; 

void
skt_destroy(skt_t *lst, skt_finalize_callback cb) {
   skt_node_t *p, *q;
   if (lst) {
      p = lst->header;
      do {
         q = p->forward[0];
         if (cb) {
            cb(p->key, p->value);
         }
         mm_free(p);
         p = q;
      } while (p != lst->NIL);
      mm_free(lst->NIL);
      mm_free(lst);
   }
}

int _randomLevel(skt_t *lst) {
   int level = 0;
   int b;
   do {
      b = lst->randomBits & 3;
      if (!b) {
         level++;
      }
      lst->randomBits>>=2;
      if (--lst->randomsLeft == 0) {
         lst->randomBits = random();
         lst->randomsLeft = BitsInRandom/2;
      };
   } while (!b);
   return (level>MaxLevel ? MaxLevel : level);
}

int
skt_insert(skt_t *lst, uint32_t key, void *value) {
   int k;
   skt_node_t *update[MaxNumberOfLevels];
   skt_node_t *p,*q;

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
         //q->value = value;
         return 0;
      }

      k = _randomLevel(lst);
      if (k > lst->level) {
         k = ++lst->level;
         update[k] = lst->header;
      };
      q = _newNodeOfLevel(k);
      q->key = key;
      q->value = value;
      do {
         p = update[k];
         q->forward[k] = p->forward[k];
         p->forward[k] = q;
      } while(--k >= 0);

      return 1;
   }
   return 0;
}

int 
skt_remove(skt_t *lst, uint32_t key) {
   int k,m;
   skt_node_t *update[MaxNumberOfLevels];
   skt_node_t *p, *q;

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
         mm_free(q);
         while( lst->header->forward[m]==lst->NIL && m>0 ) {
            m--;
         }
         lst->level = m;
         return 1;
      }
   }
   return 0;
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
      } while (--k >= 0);

      if (q->key == key) {
         return q->value;
      }
   }
   return NULL;
}

#include <stdio.h>

int main() {
   skt_t *l;
   register int i, k;
   uint32_t *keys;
   int sampleSize = 65536;

   keys = mm_malloc(sizeof(uint32_t) * sampleSize);

   l = skt_create();

   for (k=0;k<sampleSize;k++) {
      keys[k] = random();
      if ( !skt_insert(l, keys[k], &keys[k]) ) {
         printf("fail to insert %d!\n", k);
      }
   };


   for (i=0;i<4;i++) {
      for (k=0; k<sampleSize; k++) {
         uint32_t *v = skt_query(l, keys[k]);
         if (v == NULL) {
            printf("error in search #%d,#%d\n",i,k);
         } else if (*v != keys[k]) {
            printf("search returned wrong value\n");
         }
      };
      for (k=0; k<sampleSize; k++) {
         if ( !skt_remove(l, keys[k]) ) {
            printf("error in remove\n");
         }
         keys[k] = random();
         skt_insert(l, keys[k], &keys[k]);
      };
   };

   skt_destroy(l, NULL);
   mm_free(keys);

   mm_report(0);
}
