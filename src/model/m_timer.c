/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <stdio.h>
#include "m_mem.h"
#include "m_skiplist.h"
#include "m_timer.h"
#include "mfoundation_import.h"

#if M_FOUNDATION_IMPORT_MODEL_TIMER

struct s_tmr {
   skt_t *timer_lst;
};

struct s_tmr_timer {
   
   int64_t fire_ti;             /* fire date */
   int64_t interval_ti;         /* interval */
   
   unsigned char repeat;        /* repeat mode */
   tmr_callback cb;             /* call back */
   
   void *opaque;                /* user data */
};

static inline uint32_t
_timer_key(tmr_timer_t *c) {
   return (c->fire_ti & SKT_KEY_MASK);
}


tmr_t*
tmr_create_lst(void) {
   
   tmr_t *tmr = (tmr_t*)mm_malloc(sizeof(*tmr));
   if (tmr) {
      tmr->timer_lst = skt_create();
   }
   return tmr;
}


void
tmr_destroy_lst(tmr_t *tmr) {
   
   if (tmr) {
      
      while (skt_count(tmr->timer_lst) > 0) {
         mm_free(skt_popf(tmr->timer_lst));
      }
      skt_destroy(tmr->timer_lst);
      mm_free(tmr);
   }
}


void
tmr_update_lst(tmr_t *tmr, int64_t current_ti) {
   
   if (tmr==NULL || skt_count(tmr->timer_lst)<=0) {
      return;
   }

   // check first fire date
   tmr_timer_t *ti = (tmr_timer_t*)skt_first(tmr->timer_lst);
   if (ti->fire_ti > current_ti) {
      return;
   }

   // check timer in list
   while (skt_count(tmr->timer_lst) > 0) {
      tmr_timer_t *c = (tmr_timer_t*)skt_popf(tmr->timer_lst);

      if (c->fire_ti <= current_ti) {

         c->cb(c->opaque);

         if ( c->repeat ) {
            
            while (c->fire_ti <= current_ti) {
               c->fire_ti += c->interval_ti;
            }
            
            while ( !skt_insert(tmr->timer_lst, _timer_key(c), c) ) {
               c->fire_ti += 1;
            }
         }
         else {
            skt_remove(tmr->timer_lst, _timer_key(c));
            mm_free(c);
         }
      }
      else {
         break;
      }
   }
}


tmr_timer_t*
tmr_add(tmr_t *tmr,
        int64_t current_ti,
        int64_t interval_ti,
        int repeat,
        void *opaque,
        tmr_callback cb)
{
   if (tmr && cb) {
      
      tmr_timer_t *n = (tmr_timer_t*)mm_malloc(sizeof(*n));
      
      n->fire_ti = current_ti + interval_ti;
      n->interval_ti = interval_ti;
      
      n->repeat = repeat;
      n->opaque = opaque;
      
      n->cb = cb;

      while ( !skt_insert(tmr->timer_lst, _timer_key(n), n) ) {
         n->fire_ti += 1;
      }
      return n;
   }
   return NULL;
}


void
tmr_fire(tmr_t *tmr,
         tmr_timer_t *c,
         int64_t current_ti,
         int run_callback)
{
   if (tmr && c) {
      
      skt_remove(tmr->timer_lst, _timer_key(c));

      if (run_callback) {
         c->cb(c->opaque);
      }

      if ( !c->repeat ) {
         mm_free(c);
         return;
      }

      c->fire_ti = current_ti + c->interval_ti;
      while ( !skt_insert(tmr->timer_lst, _timer_key(c), c) ) {
         c->fire_ti += 1;
      }
   }
}


void
tmr_invalidate(tmr_t *tmr, tmr_timer_t *c) {
   
   if (tmr && c) {
      skt_remove(tmr->timer_lst, _timer_key(c));
      mm_free(c);
   }
}

#endif  /* M_FOUNDATION_IMPORT_MODEL_TIMER */
