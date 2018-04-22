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
#include "m_skiplist.h"
#include "mfoundation_import.h"

#if M_FOUNDATION_IMPORT_MODEL_TIMER

/* schedule unit */
typedef struct {
   int64_t fire_ti;             /* fire date */
   lst_t *tm_lst;               /* timer list */
} tmr_unit_t;

/* list of schedule unit */
struct s_tmr {
   skt_t *unit_skt;             /* sched unit */
};

/* callback timer */
struct s_tmr_timer {
   int64_t interval_ti;         /* interval */   
   unsigned char repeat;        /* repeat mode */
   tmr_callback cb;             /* call back */
   void *opaque;                /* user data */

   tmr_unit_t *unit;            /* unit in unit_skt */
   lst_node_t *node;            /* node in tm_lst */
};


/* create unit list */
tmr_t*
tmr_create_lst(void) {
   
   tmr_t *tmr = (tmr_t*)mm_malloc(sizeof(*tmr));
   if (tmr) {
      tmr->unit_skt = skt_create();
   }
   return tmr;
}


/* create unit */
static tmr_unit_t*
_tmr_unit_create(int64_t fire_ti) {
   
   tmr_unit_t *u = (tmr_unit_t*)mm_malloc(sizeof(*u));
   if (u) {
      
      u->fire_ti = fire_ti;
      u->tm_lst = lst_create();
   }
   return u;
}


/* remove unit from schedule list */
static int
_tmr_unit_remove(tmr_t *tmr, tmr_unit_t *u) {
   
   if ( u ) {
      
      if (lst_count(u->tm_lst) <= 0) {
         skt_remove(tmr->unit_skt, u->fire_ti);
         return 1;
      }
   }
   return 0;
}


/* destroy unit */
static void
_tmr_unit_destroy(tmr_unit_t *u) {
   
   if ( u ) {
      
      while (lst_count(u->tm_lst) > 0) {
         mm_free( lst_popf(u->tm_lst) );
      }
      
      lst_destroy(u->tm_lst);
      mm_free(u);
   }
}


/* destroy unit list */
void
tmr_destroy_lst(tmr_t *tmr) {
   
   if (tmr) {
      
      while (skt_count(tmr->unit_skt) > 0) {
         tmr_unit_t *u = (tmr_unit_t*)skt_popf(tmr->unit_skt);
         _tmr_unit_destroy( u );
      }
      
      skt_destroy(tmr->unit_skt);
      mm_free(tmr);
   }
}


/* remove tm, then check & remove unit */
static int
_tmr_tm_remove(tmr_t *tmr, tmr_timer_t *tm) {
   if (tmr && tm) {
      lst_remove(tm->unit->tm_lst, tm->node);
   }
   return 0;
}


/* add a timer callback to proper schedule unit */
static tmr_timer_t*
_tmr_add(tmr_t *tmr,
         tmr_timer_t *tm,
         int64_t current_ti)
{
   int64_t fire_ti = current_ti + tm->interval_ti;
   tmr_unit_t *u = skt_query(tmr->unit_skt, fire_ti);
   
   if (u == NULL) {
      u = _tmr_unit_create(fire_ti);
      skt_insert(tmr->unit_skt, fire_ti, u);
   }

   tm->unit = u;
   tm->node = lst_pushl(u->tm_lst, tm);
   return tm;
}


/* check fire date, update schedule unit */
void
tmr_update_lst(tmr_t *tmr, int64_t current_ti) {
   
   if (tmr==NULL || skt_count(tmr->unit_skt)<=0) {
      return;
   }

   // check first fire unit
   tmr_unit_t *u = (tmr_unit_t*)skt_first(tmr->unit_skt);
   if (u->fire_ti > current_ti) {
      return;
   }
   
   // check timer in list
   for (int i=0; i<skt_count(tmr->unit_skt); i++) {
      
      tmr_unit_t *u = skt_first(tmr->unit_skt);
      if (u->fire_ti > current_ti) {
         break;
      }

      do {
         tmr_timer_t *tm = (tmr_timer_t*)lst_popf(u->tm_lst);

         tm->cb(tm->opaque);
         
         if (tm->repeat) {
            _tmr_add(tmr, tm, current_ti);
         } else {
            mm_free(tm);               
         }
         
      } while (lst_count(u->tm_lst) > 0);

      _tmr_unit_remove(tmr, u);
      _tmr_unit_destroy(u);
   }
}


/* add a timer callback to tmr */
tmr_timer_t*
tmr_add(tmr_t *tmr,
        int64_t current_ti,
        int64_t interval_ti,
        int repeat,
        void *opaque,
        tmr_callback cb)
{
   if (tmr && cb) {
      
      tmr_timer_t *tm = (tmr_timer_t*)mm_malloc(sizeof(*tm));
      if ( tm ) {
         tm->interval_ti = interval_ti;
         tm->repeat = repeat;
         tm->cb = cb;
         tm->opaque = opaque;
         return _tmr_add(tmr, tm, current_ti);
      }
   }
   return NULL;
}


/* fire a timer callback */
void
tmr_fire(tmr_t *tmr,
         tmr_timer_t *tm,
         int64_t current_ti,
         int run_callback)
{
   if (tmr && tm) {

      _tmr_tm_remove(tmr, tm);

      if ( _tmr_unit_remove(tmr, tm->unit) ) {
         _tmr_unit_destroy(tm->unit);
      }      
      
      if (run_callback) {
         tm->cb(tm->opaque);
      }

      if ( !tm->repeat ) {
         mm_free(tm);
         return;
      }

      _tmr_add(tmr, tm, current_ti);
   }
}


/* invalidate a timer callback */
void
tmr_invalidate(tmr_t *tmr, tmr_timer_t *tm) {
   
   if (tmr && tm) {
      _tmr_tm_remove(tmr, tm);
      if ( _tmr_unit_remove(tmr, tm->unit) ) {
         _tmr_unit_destroy(tm->unit);
      }
      mm_free(tm);
   }
}

#endif  /* M_FOUNDATION_IMPORT_MODEL_TIMER */
