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

/* schedule unit, free unit & tm */
struct s_tmr {
   skt_t *unit_skt;             /* sched unit */
   lst_t *unit_free;            /* free unit */
   lst_t *tm_free;              /* free tm */
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
      tmr->unit_free = lst_create();
      tmr->tm_free = lst_create();
   }
   return tmr;
}


/* create and active unit  */
static tmr_unit_t*
_tmr_unit_create(tmr_t *tmr, int64_t fire_ti) {
   
   tmr_unit_t *u = NULL;

   if (lst_count(tmr->unit_free) > 0) {
      u = (tmr_unit_t*)lst_popf(tmr->unit_free);
   } else {
      u = (tmr_unit_t*)mm_malloc(sizeof(*u));
      u->tm_lst = lst_create();
   }

   u->fire_ti = fire_ti;
   skt_insert(tmr->unit_skt, fire_ti, u);

   return u;
}


/* suspend unit when no timer left */
static void
_tmr_unit_try_suspend(tmr_t *tmr, tmr_unit_t *u) {

   if (lst_count(u->tm_lst) <= 0) {

      skt_remove(tmr->unit_skt, u->fire_ti);
      lst_pushl(tmr->unit_free, u);
   }
}


/* destroy unit, free every timer */
static void
_tmr_unit_destroy(tmr_unit_t *u) {

   while (lst_count(u->tm_lst) > 0) {
      mm_free( lst_popf(u->tm_lst) );
   }

   lst_destroy(u->tm_lst);
   mm_free(u);
}


/* destroy unit list */
void
tmr_destroy_lst(tmr_t *tmr) {
   
   if (tmr) {
      
      while (skt_count(tmr->unit_skt) > 0) {
         _tmr_unit_destroy( (tmr_unit_t*)skt_popf(tmr->unit_skt) );
      }
      skt_destroy(tmr->unit_skt);

      while (lst_count(tmr->unit_free) > 0) {
         _tmr_unit_destroy( (tmr_unit_t*)lst_popf(tmr->unit_free) );
      }
      lst_destroy(tmr->unit_free);

      while (lst_count(tmr->tm_free) > 0) {
         mm_free( lst_popf(tmr->tm_free) );
      }
      lst_destroy(tmr->tm_free);

      mm_free(tmr);
   }
}


/* create tm */
static tmr_timer_t*
_tmr_tm_create(tmr_t *tmr) {
   tmr_timer_t *tm = NULL;
   if (lst_count(tmr->tm_free) > 0) {
      tm = (tmr_timer_t*)lst_popf(tmr->tm_free);
   } else {
      tm = (tmr_timer_t*)mm_malloc(sizeof(*tm));
   }
   return tm;
}


/* suspend tm, remove tm from unit */
static inline void
_tmr_tm_suspend(tmr_t *tmr, tmr_timer_t *tm) {
   lst_remove(tm->unit->tm_lst, tm->node);
}

/* recycle to tm_free */
static inline void
_tmr_tm_recycle(tmr_t *tmr, tmr_timer_t *tm) {
   lst_pushl(tmr->tm_free, tm);
}


/* add a timer callback to proper schedule unit */
static tmr_timer_t*
_tmr_add_tm(tmr_t *tmr,
            tmr_timer_t *tm,
            int64_t current_ti)
{
   int64_t fire_ti = current_ti + tm->interval_ti;
   tmr_unit_t *u = (tmr_unit_t*)skt_query(tmr->unit_skt, fire_ti);
   
   if (u == NULL) {
      u = _tmr_unit_create(tmr, fire_ti);
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
      
      tmr_unit_t *u = (tmr_unit_t*)skt_first(tmr->unit_skt);
      if (u->fire_ti > current_ti) {
         break;
      }

      do {
         tmr_timer_t *tm = (tmr_timer_t*)lst_popf(u->tm_lst);

         tm->cb(tm, tm->opaque);
         
         if (tm->repeat) {
            _tmr_add_tm(tmr, tm, current_ti);
         } else {
            _tmr_tm_recycle(tmr, tm);
         }
         
      } while (lst_count(u->tm_lst) > 0);

      _tmr_unit_try_suspend(tmr, u);
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
      
      tmr_timer_t *tm = _tmr_tm_create(tmr);
      if ( tm ) {
         tm->interval_ti = interval_ti;
         tm->repeat = repeat;
         tm->cb = cb;
         tm->opaque = opaque;
         return _tmr_add_tm(tmr, tm, current_ti);
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
   if (tmr && tm && tm->unit) {

      _tmr_tm_suspend(tmr, tm);
      _tmr_unit_try_suspend(tmr, tm->unit);
      
      if (run_callback) {
         tm->cb(tm, tm->opaque);
      }

      if ( !tm->repeat ) {
         _tmr_tm_recycle(tmr, tm);
         return;
      }

      _tmr_add_tm(tmr, tm, current_ti);
   }
}


/* invalidate a timer callback */
void
tmr_invalidate(tmr_t *tmr, tmr_timer_t *tm) {
   
   if (tmr && tm && tm->unit) {
      _tmr_tm_suspend(tmr, tm);
      _tmr_tm_recycle(tmr, tm);
      _tmr_unit_try_suspend(tmr, tm->unit);
   }
}

#endif  /* M_FOUNDATION_IMPORT_MODEL_TIMER */
