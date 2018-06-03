/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#ifdef M_FOUNDATION_TEST_RS

#include <stdio.h>
#include <stdlib.h>

#include "m_rs.h"
#include "m_mem.h"

// add up to t errors
static void
_add_errors(unsigned char *m, unsigned char *c, int n, int t) {
   int i, j;

   for(i=0; i<t; i++) {
      j = rand() % n;
      if (i < (t<<1)) {
         c[j] = rand() & 0xFF;
      } else {
         m[j - (t<<1)] = rand() % 0xFF;
      }
   }
}

static void
_test_codec(void) {
   unsigned char org[223];
   unsigned char m[223];
   unsigned char c[32];   

   const int k = 223;           /* msg info length */
   const int t2 = 32;           /* max error bytes */
   const int n = k + t2;        /* total msg length */
   int i, j;
    
   rs_t *rs = rs_init(k, t2);
   if ( !rs ) {
      abort();
   }
    
   srand((unsigned)rs);
    
   for(i=0; i<10000; i++) {
        
      for(j=k-1; j>=0; j--) {
         org[j] = m[j] = rand() & 0xFF;
      }
        
      if ( rs_encode(rs, m, c) ) {
            
         _add_errors(m, c, (k + t2), t2>>1);
            
         int ret = rs_decode(rs, m, c);
            
         int errors = 0;
         for(j=0; j<k; j++) {
            errors += org[j] != m[j];
         }

         printf("RS(%d, %d, %d), ret=%d, err=%d ", n, k, t2, ret, errors);
            
         if (!ret && errors>0) {
            printf(" %d errors undetected\n", errors);
         } else {
            printf("\n");
         }
      }
   }
   
   rs_fini(rs);
   mm_report(0);
}

int main(int argc, char *argv[]) {
   _test_codec();
   return 0;
}

#endif // M_FOUNDATION_TEST_RS
