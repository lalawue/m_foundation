/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <stdio.h>
#include "m_mem.h"
#include "m_list.h"
#include "m_timer.h"


struct s_tmr {
   int64_t first_ti;            /* first fire date */
   lst_t *timer_lst;
};

struct s_tmr_timer {
   
   int64_t fire_ti;             /* fire date */
   int64_t interval_ti;         /* interval */
   
   unsigned char repeat;        /* repeat mode */
   tmr_callback cb;             /* call back */
   
   void *opaque;                /* user data */
   void *node;                  /* node in list */
};


tmr_t*
tmr_create_lst(void) {
   
   tmr_t *tmr = (tmr_t*)mm_malloc(sizeof(*tmr));
   if (tmr) {
      
      tmr->timer_lst = lst_create();      
   }
   return tmr;
}


void
tmr_destroy_lst(tmr_t *tmr) {
   
   if (tmr) {
      
      while (lst_count(tmr->timer_lst) > 0) {
         
         mm_free(lst_popf(tmr->timer_lst));
      }
      
      lst_destroy(tmr->timer_lst);
      mm_free(tmr);
   }
}


static inline void
_update_first_ti(tmr_t *tmr, int64_t fire_ti) {
   if (lst_count(tmr->timer_lst) <= 0) {
      tmr->first_ti = 0;
   }
   else if (!tmr->first_ti || (tmr->first_ti > fire_ti)) {
      tmr->first_ti = fire_ti;
   }
}


void
tmr_update_lst(tmr_t *tmr, int64_t current_ti) {
   
   if (tmr==NULL || lst_count(tmr->timer_lst)<=0) {
      return;
   }

   // check first fire date
   if (tmr->first_ti && (tmr->first_ti > current_ti)) {
      return;
   }

   // get the max
   int64_t first_ti = ((tmr_timer_t*)lst_last(tmr->timer_lst))->fire_ti;

   // check timer in list
   lst_foreach(it, tmr->timer_lst) {
      tmr_timer_t *c = lst_iter_data(it);
            
      if (c->fire_ti <= current_ti) {
               
         c->fire_ti = current_ti + c->interval_ti;
         c->cb(c->opaque);
               
         if ( !c->repeat ) {

            lst_iter_remove(it);
            mm_free(c);
            continue;
         }
      }

      // get the min
      if (first_ti > c->fire_ti) {
         first_ti = c->fire_ti;
      }
   }

   _update_first_ti(tmr, first_ti);
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
      
      tmr_timer_t *n = mm_malloc(sizeof(*n));
      
      n->fire_ti = current_ti + interval_ti;
      n->interval_ti = interval_ti;
      
      n->repeat = repeat;
      n->opaque = opaque;
      
      n->cb = cb;

      n->node = lst_pushl(tmr->timer_lst, n);

      _update_first_ti(tmr, n->fire_ti);
      return n;
   }
   return NULL;
}


void
tmr_fire(tmr_t *tmr,
         tmr_timer_t *c,
         int64_t ti,
         int run_callback)
{
   if (tmr && c) {
      
      if (run_callback) {
         c->cb(c->opaque);
      }

      if (tmr->first_ti == c->fire_ti) {
         tmr->first_ti = 0;
      }
      
      if ( !c->repeat ) {
         lst_remove(tmr->timer_lst, c->node);
         mm_free(c);
         return;
      }

      c->fire_ti = ti + c->interval_ti;
   }
}


void
tmr_invalidate(tmr_t *tmr, tmr_timer_t *c) {
   
   if (tmr && c) {

      if (tmr->first_ti == c->fire_ti) {
         tmr->first_ti = 0;
      }
      
      lst_remove(tmr->timer_lst, c->node);
      mm_free(c);
   }
}
