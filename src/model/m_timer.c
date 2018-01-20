/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <stdio.h>
#include "m_timer.h"
#include "m_mem.h"
#include "plat_time.h"

struct s_tmr {
   int64_t first_ms;            /* first fire date */
   lst_t *timer_lst;
};

struct s_tmr_timer {
   
   int64_t fire_ms;             /* fire date */
   int64_t interval_ms;         /* interval */
   
   unsigned char repeat;        /* repeat mode */
   tmr_callback cb;             /* call back */
   
   void *opaque;                /* user data */
   void *node;                  /* node in list */
};


tmr_t*
tmr_create_lst(void) {
   
   tmr_t *tmr = mm_malloc(sizeof(*tmr));
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


void
tmr_update_lst(tmr_t *tmr) {
   
   if (tmr==NULL || lst_count(tmr->timer_lst)<=0) {
      return;
   }

   int64_t current = mtime_current();

   // check first fire date
   if (ls->fire_ms && ls->fire_ms>current) {
      return;
   }

   // check timer in list, break when fire date not reached
   lst_foreach(it, tmr->timer_lst) {
      c = lst_iter_data(it);
            
      if (c->fire_ms <= current) {
               
         c->fire_ms = current + c->interval_ms;
         c->cb(c->opaque);
               
         if ( !c->repeat ) {
            
            lst_iter_remove(it);
            mm_free(c);
         }
      }
      else {
         if (!ls->fire_ms || ls->fire_ms>c->fire_ms) {
            ls->fire_ms = c->fire_ms;
         }
      }
   }
   
}


tmr_timer_t*
tmr_add(tmr_t *tmr, int64_t interval_ms, int repeat, void *opaque, tmr_callback cb) {
   
   if (tmr && cb) {
      
      tmr_timer_t *n = mm_malloc(sizeof(*n));
      
      n->fire_ms = mtime_current() + interval_ms;
      n->interval_ms = interval_ms;
      
      n->repeat = repeat;
      n->opaque = opaque;
      
      n->cb = cb;

      n->node = lst_pushl(tmr->timer_lst, n);

      if (!lst->fire_ms || n->fire_ms<lst->fire_ms) {
         lst->fire_ms = n->fire_ms;
      }
      
      return n;
   }
   return NULL;
}


void
tmr_fire(tmr_t *tmr, tmr_timer_t *timer, int run_callback) {
   
   if (tmr && timer) {
      
      if (run_callback) {
         timer->cb(timer->opaque);
      }

      if (lst->fire_ms == timer->fire_ms) {
         lst->fire_ms = 0;
      }
      
      if ( !timer->repeat ) {
         lst_remove(tmr->timer_lst, timer->node);
         mm_free(timer);
         return;
      }

      timer->fire_ms = mtime_current() + timer->interval_ms;      
   }
}


void
tmr_invalidate(tmr_t *tmr, tmr_timer_t *timer) {
   
   if (tmr && timer) {

      if (lst->fire_ms == timer->fire_ms) {
         lst->fire_ms = 0;
      }
      
      lst_remove(tmr->timer_lst, timer->node);
      mm_free(timer);
   }
}
