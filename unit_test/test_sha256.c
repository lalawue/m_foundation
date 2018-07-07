/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifdef M_FOUNDATION_TEST_SHA256

#include <stdio.h>
#include <stdlib.h>
#include "m_sha256.h"

int main(int argc, char *argv[])
{
   if (argc < 2) {
      printf("%s FILE\n", argv[0]);
      return 0;
   }

   uint8_t hash[SHA256_HASH_BYTES];
   size_t buf_len;
   unsigned char *buf;   

   {
      FILE *fp = fopen(argv[1], "r");
      if (!fp) {
         printf("fail to open file '%s'\n", argv[1]);
         return 0;
      }

      fseek(fp, 0, SEEK_END);
      buf_len = ftell(fp);
      rewind(fp);

      buf = malloc(buf_len);
      fread(buf, buf_len, 1, fp);

      fclose(fp);
   }

   sha256_once((const void*)buf, (size_t)buf_len, (uint8_t*)hash);

   for (int i=0; i<SHA256_HASH_BYTES; i++) {
      printf("%02x", hash[i]);
   }
   printf("  %s\n", argv[1]);

   free(buf);
   return 0;
}

#endif  // M_FOUNDATION_TEST_SHA256
