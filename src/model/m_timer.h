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


typedef void (*tmr_callback)(void *opaque);
   

typedef struct s_tmr tmr_t;
typedef struct s_tmr_timer tmr_timer_t;   



tmr_t* tmr_create_lst(void);

void tmr_destroy_lst(tmr_t*);

   
// input current time unit, and check timers' fire date
void tmr_update_lst(tmr_t*, int64_t current_ti);


tmr_timer_t* tmr_add(tmr_t*,
                     int64_t current_ti,  /* current time unit */
                     int64_t interval_ti, /* interval time unit */
                     int repeat,          /* repeat mode */
                     void *opaque,        /* user data */
                     tmr_callback cb);    /* timer callback */

void tmr_fire(tmr_t*,
              tmr_timer_t*,
              int64_t current_ti, /* current time unit */
              int run_callback);  /* run callback */

   
// suspend timer
void tmr_invalidate(tmr_t*, tmr_timer_t*);


#ifdef __cplusplus
}
#endif

#endif
