/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef M_TIMER_H
#define M_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "m_list.h"


typedef void (*tmr_callback)(void*);


typedef struct {
   int64_t last_ms;
   lst_t *timer_lst;
} tmr_t;


typedef struct {
   int64_t fire_ms;
   int64_t interval_ms;
   unsigned char repeat;
   tmr_callback cb;
   void *opaque;
   void *node;
} tmr_timer_t;



tmr_t* tmr_create_lst(void);

void tmr_destroy_lst(tmr_t*);

// loop timer list and fire
void tmr_update_lst(tmr_t*);




tmr_timer_t* tmr_add(tmr_t*, int64_t interval_ms, int repeat, void *opaque, tmr_callback cb);

void tmr_fire(tmr_t*, tmr_timer_t*, int run_callback);

// remove timer from list
void tmr_invalidate(tmr_t*, tmr_timer_t*);



#ifdef __cplusplus
}
#endif

#endif
