/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifdef M_FOUNDATION_TEST_TIMER

#include <stdio.h>
#include <unistd.h>
#include "m_timer.h"

static void
_test_tmr_callback(void *opaque) {
   printf("tmr callback %d\n", *((int*)opaque));
}

int main(int argc, char *argv[]) {

   int value[3] = {0, 1, 2};
   tmr_timer_t *tm[3];
   
   tmr_t *tmr = tmr_create_lst();

   for (int i=0; i<20; i++) {
      
      if (i <= 2) {
         tm[i] = tmr_add(tmr, i, 1+i, 1, &value[i], _test_tmr_callback);
      }

      usleep(500000);

      tmr_update_lst(tmr, i);

      if (i == 10) {
         tmr_invalidate(tmr, tm[0]);
      }
      else if (i == 16) {
         tmr_fire(tmr, tm[1], i, 1);
      }
   }

   tmr_destroy_lst(tmr);
   
   return 0;
}

#endif
