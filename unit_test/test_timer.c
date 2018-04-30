/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifdef M_FOUNDATION_TEST_TIMER

#include <stdio.h>
#include <unistd.h>
#include "m_mem.h"
#include "m_timer.h"

static void
_test_tmr_callback(tmr_timer_t *tm, void *opaque) {
   printf("cb %d<%p>, ", *((int*)opaque), tm);
}

int main(int argc, char *argv[]) {

   int value[4] = {0, 1, 2, 3};
   tmr_timer_t *tm[3];
   
   tmr_t *tmr = tmr_create_lst();

   for (int i=0; i<21; i++) {
      
      if (i <= 2) {
         tm[i] = tmr_add(tmr, i, 1+i, 1, &value[i], _test_tmr_callback);
         if (i == 2) {
            int j = i + 1;
            tm[j] = tmr_add(tmr, i, 2*(j+1), 1, &value[j], _test_tmr_callback);
         }
      }

      usleep(500000);

      printf("%02d ", i);
      tmr_update_lst(tmr, i);

      if (i == 10) {
         tmr_invalidate(tmr, tm[0]);
      }
      else if (i == 16) {
         tmr_fire(tmr, tm[1], i, 1);
      }

      printf("\n");
   }

   tmr_destroy_lst(tmr);
   mm_report(2);
   return 0;
}

#endif
