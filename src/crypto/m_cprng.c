/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include "plat_os.h"
#include "m_cprng.h"
#include "mfoundation_import.h"

#if M_FOUNDATION_IMPORT_CRYPTO_CPRNG

#ifdef PLAT_OS_WIN

#include <wincrypt.h>
#pragma comment(lib, "Advapi32.lib")

#else

#include <stdio.h>

#endif

int
cprng_random(unsigned char *buf, int buf_len) {
   
   if (buf==NULL || buf_len<=0) {
      return 0;
   }
   
   int ret = 0;
   
#ifdef PLAT_OS_WIN
   
   for (int i=0; i<2; i++) {
      HCRYPTPROV hCryptProv = NULL;
      if ( CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0) ) {
         if ( CryptGenRandom(hCryptProv, (DWORD)bufLen, (BYTE*)buf) ) {
            ret = 1;
         }
         CryptReleaseContext(hCryptProv, 0);
         if (ret) { break; }
      }
   }
   
#else
   
   FILE *fp = fopen("/dev/random", "r");
   if ( fp ) {
      ret = fread(buf, buf_len, 1, fp);
      fclose(fp);
   }
   
#endif
   
   return ret;
}

#endif  /* M_FOUNDATION_IMPORT_PLAT_RANDOM */
