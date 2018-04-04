/* 
 * Copyright (c) 2017 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <string.h>
#include "m_xor64.h"
#include "mfoundation_import.h"

#if M_FOUNDATION_IMPORT_CRYPTO_XOR64

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
xor64_checksum_gen(uint8_t *buf, int len, uint8_t *checksum) {
   if (buf && checksum && len>=16) {
      uint64_t ret = _64bit_xor(buf, len);
      memcpy(checksum, &ret, sizeof(ret));
      return 1;
   }
   return 0;
}

int
xor64_checksum_check(uint8_t *buf, int len, uint8_t *checksum) {
   if (buf && checksum && len>=16) {
      uint64_t ret = _64bit_xor(buf, len);
      return (0 == memcmp((void*)&ret, (void*)checksum, sizeof(ret)));
   }
   return 0;
}

#if M_XOR64_TEST

#include <stdio.h>
#include <stdlib.h>

static void
_invert_bit_offset(uint8_t *buf, int offset, int bits) {
   if (bits>=0 & bits<=7) {
      uint8_t val = buf[offset] & (1 << bits);
      if (val) { buf[offset] &= ~val; }
      else     { buf[offset] |= (1 << bits); }
   }
}

int
main(int argc, char *argv[]) {
   int buf_len = 2049;
   uint8_t buf[buf_len];

   uint8_t checksum[8];

   memset(buf, 0, buf_len);

   for (int i=0; i<buf_len; i++) {
      buf[i] = random();
   }

   if ( xor64_checksum_gen(buf, buf_len, checksum) ) {

      _invert_bit_offset(buf, buf_len-1, 7);
      _invert_bit_offset(buf, buf_len-1, 7);
      //_invert_bit_offset(buf, 889, 4);
      //_invert_bit_offset(buf, 231, 7);

      if( xor64_checksum_check(buf, buf_len, checksum) ) {
         printf("valid checksum\n");
      } else {
         printf("invalid checksum !\n");
      }

   } else {
      
      printf("fail to gen checksum\n");
   }

   return 0;
}
#endif  // M_XOR64_TEST

#endif  // M_FOUNDATION_IMPORT_CRYPTO_XOR64
