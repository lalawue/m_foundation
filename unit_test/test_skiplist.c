/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifdef M_FOUNDATION_TEST_SKIPLIST

#include <stdio.h>
#include <unistd.h>
#include "m_skiplist.h"

#define TEST_COUNT 32

int main(int argc, char *argv[]) {
   unsigned int value[TEST_COUNT];

   /* init */
   for (int i=1; i<=TEST_COUNT; i++) {
      value[i-1] = 2 * i;
   }

   skt_t *lst = skt_create();
   if (lst) {
      for (unsigned int i=1; i<=TEST_COUNT; i++) {
         skt_insert(lst, 2*i, &value[i-1]);
      }

      unsigned int *v = skt_first(lst);
      printf("%u\n", *v);

      v = skt_popf(lst);
      printf("%u\n", *v);

      v = skt_first(lst);
      printf("%u\n", *v);

      v = skt_query(lst, 6);
      printf("%u\n", *v);

      v = skt_query(lst, TEST_COUNT-2);
      if (v) {
         printf("%u\n", *v);
         skt_remove(lst, TEST_COUNT - 2);
         if (v) {
            printf("count:%d\n", skt_count(lst));
         }
      }
      
      if ( skt_remove(lst, 2) ) {
         v = skt_first(lst);
         printf("%u\n", *v);
      }

      unsigned int tempValue = 999;
      skt_insert(lst, TEST_COUNT - 2, &tempValue);
      
      skt_foreach(it, lst) {
         unsigned int *m = it->value;
         printf("%u, ", *m);
         skt_iter_remove(lst, it);
      }
      printf("\ncount:%d\n", skt_count(lst));
      
      skt_destroy(lst);
   }
   
   return 0;
}

#endif
