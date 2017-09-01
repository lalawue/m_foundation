/* 
 * Copyright (c) 2017 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <string.h>
#include "m_xor64.h"

static uint64_t
_64bit_xor(uint8_t *buf, int bytes) {
   uint64_t *p=(uint64_t*)buf, ret=0;
   for (int i=0; i<(bytes>>3); i++) {
      ret = ret ^ p[i];
   }
   if (bytes & 0x7) {
      uint8_t extra[8];
      memset(extra, 0, sizeof(uint8_t)*8);
      memcpy(extra, &buf[bytes & ~0x7], bytes & 0x7);
      ret = ret ^ *((uint64_t*)extra);
   }
   return ret;
}

int
xor64_checksum_gen(uint8_t *buf_head_8bytes_for_checksum, int len) {
   if (buf_head_8bytes_for_checksum && (len >= 16)) {
      uint8_t *checksum = buf_head_8bytes_for_checksum;
      uint8_t *input = &buf_head_8bytes_for_checksum[8];
      uint64_t ret = _64bit_xor(input, len - 8);
      memcpy(checksum, &ret, sizeof(ret));
      return 1;
   }
   return 0;
}

int
xor64_checksum_check(uint8_t *buf_input, int len) {
   if (buf_input && len>=16) {
      uint8_t *input = &buf_input[8];
      uint64_t ret = _64bit_xor(input, len - 8);
      return (0 == memcmp((void*)&ret, (void*)buf_input, sizeof(ret)));
   }
   return 0;
}

#if M_XOR64_TEST

#include <stdio.h>
#include <stdlib.h>

static void
_invert_bit_offset(uint8_t *buf, int offset, int bits) {
   uint8_t val = buf[offset] & (1 << bits);
   if (val) { buf[offset] &= ~val; }
   else     { buf[offset] |= (1 << bits); }
}

int
main(int argc, char *argv[]) {
   uint8_t buf[1024];
   memset(buf, 0, 1024);

   for (int i=0; i<1024; i++) {
      buf[i] = random();
   }

   if ( xor64_checksum_gen(buf, 1024) ) {

      _invert_bit_offset(buf, 21, 0);
      _invert_bit_offset(buf, 21, 0);
      /* _invert_bit_offset(buf, 889, 4); */
      /* _invert_bit_offset(buf, 231, 7); */


      if( xor64_checksum_check(buf, 1024) ) {
         printf("valid checksum\n");
      } else {
         printf("invalid checksum !\n");
      }

   } else {
      
      printf("fail to gen checksum\n");
   }

   return 0;
}
#endif //M_XOR64_TEST
