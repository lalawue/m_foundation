/* 
 * Copyright (c) 2015 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plat_lock.h"
#include "m_mem.h"
#include "m_list.h"
#include "m_stm.h"

struct s_stm {
   lock_t lock;
   lst_t *data_lst;
   stm_finalizer finalizer;
   void *ud;
   char name[1];
};

typedef struct {
   lock_t lock;
   int init;
   lst_t *stm_lst;
} global_stm_t;

static global_stm_t _g_stm;

void stm_init(void) {
   global_stm_t *gs = &_g_stm;
   if ( !gs->init ) {
      gs->init = 1;
      gs->stm_lst = lst_create();
   }
}

void stm_fini(void) {
   global_stm_t *gs = &_g_stm;
   if (gs->init) {
      while (lst_count(gs->stm_lst) > 0) {
         stm_t *s = (stm_t*)lst_popf(gs->stm_lst);
         stm_clear(s);
         lst_destroy(s->data_lst);
         mm_free(s);
      }
      lst_destroy(gs->stm_lst);
      gs->init = 0;
   }
}

stm_t*
stm_create(const char *name, stm_finalizer f, void *ud) {
   if (name == NULL) return NULL;
   if ( !stm_retrive(name) ) {
      global_stm_t *gs = &_g_stm;
      stm_t *s = (stm_t*)mm_malloc(sizeof(*s) + (unsigned)strlen(name));
      strcpy(s->name, name);
      s->finalizer = f;
      s->ud = ud;
      s->data_lst = lst_create();

      _lock(gs->lock);
      lst_pushl(gs->stm_lst, s);
      _unlock(gs->lock);
      return s;
   }
   return NULL;
}

stm_t*
stm_retrive(const char *name) {
   global_stm_t *gs = &_g_stm;
   _lock(gs->lock);
   lst_foreach(its, gs->stm_lst) {
      stm_t *ss = (stm_t*)lst_iter_data(its);
      if (strcmp(ss->name, name) == 0) {
         _unlock(gs->lock);
         return ss;
      }
   }
   _unlock(gs->lock);
   return NULL;
}

static inline void
_dumb_finalizer(void *ptr, void *ud) {
}

void stm_clear(stm_t *s) {
   if ( s ) {
      _lock(s->lock);
      stm_finalizer f = s->finalizer ? s->finalizer : _dumb_finalizer;
      while (lst_count(s->data_lst) > 0) {
         void *data = lst_popf(s->data_lst);
         f(data, s->ud);
      }
      _unlock(s->lock);
   }
}

int stm_count(stm_t *s) {
   return s ? lst_count(s->data_lst) : -1;
}

int stm_pushf(stm_t *s, void *data) {
   if ( s ) {
      _lock(s->lock);
      lst_pushf(s->data_lst, data);
      _unlock(s->lock);
      return 1;
   }
   return 0;
}

int stm_pushl(stm_t *s, void *data) {
   if ( s ) {
      _lock(s->lock);
      lst_pushl(s->data_lst, data);
      _unlock(s->lock);
      return 1;
   }
   return 0;
}

void* stm_popf(stm_t *s) {
   void *data = NULL;
   if ( s ) {
      _lock(s->lock);
      data = lst_popf(s->data_lst);
      _unlock(s->lock);
   }
   return data;
}

void* stm_popl(stm_t *s) {
   void *data = NULL;
   if ( s ) {
      _lock(s->lock);
      data = lst_popl(s->data_lst);
      _unlock(s->lock);
   }
   return data;
}

int stm_total(void) {
   global_stm_t *gs = &_g_stm;
   return lst_count(gs->stm_lst);
}
