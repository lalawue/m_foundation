/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifdef M_FOUNDATION_TEST_CHACHA20

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m_chacha20.h"

int main(int argc, char *argv[]) {
   unsigned char *in_buf, *out_buf;
   int buf_len;

   {
      FILE *fp = fopen(argv[1], "r");
      if (!fp) {
         printf("fail to open file '%s'\n", argv[1]);
         return 0;
      }

      fseek(fp, 0, SEEK_END);
      buf_len = ftell(fp);
      rewind(fp);

      in_buf = malloc(buf_len + 1);
      out_buf = malloc(buf_len + 1);

      memset(in_buf, 0, buf_len);
      memset(out_buf, 0, buf_len);
      
      fread(in_buf, buf_len, 1, fp);
      fclose(fp);
   }

   chacha20_ctx_t ctx;


   /* enc
    */
   chacha20_ctx_init(&ctx);
   
   chacha20_key_setup(&ctx, (const unsigned char*)"01234567890123456789012345678901", 32);
   chacha20_iv_setup(&ctx, "01234567", 8);

   chacha20_xor(&ctx, in_buf, out_buf, buf_len);

   /* dec
    */
   chacha20_ctx_init(&ctx);

   chacha20_key_setup(&ctx, "01234567890123456789012345678901", 32);
   chacha20_iv_setup(&ctx, "01234567", 8);

   chacha20_xor(&ctx, out_buf, in_buf, buf_len);

   in_buf[buf_len] = 0;
   printf("%s", in_buf);

   free(in_buf);
   free(out_buf);   
   return 0;
}

#endif // M_FOUNDATION_TEST_CHACHA20
