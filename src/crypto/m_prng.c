/* 
 * Copyright (c) 2017 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

/* PRNG from 'http://xoroshiro.di.unimi.it/xoroshiro128plus.c'
 */

#include "plat_os.h"
#include "m_prng.h"
#include <stdio.h>


int
prng_init(prng_t *rng) {
   if (rng) {
      
#if defined(PLAT_OS_MAC) || defined(PLAT_OS_LINUX) || defined(PLAT_OS_FREEBSD)
      int res;
      FILE *fp = fopen("/dev/urandom", "rb");
      if (!fp) {
         return 0;
      }
      res = fread(rng->seed, 1, sizeof(rng->seed), fp);
      fclose(fp);
      if ( res != sizeof(rng->seed) ) {
         return 0;
      }
      
#elif defined(PLAT_OS_WIN)
      int res;
      HCRYPTPROV hCryptProv;
      res = CryptAcquireContext(
         &hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
      if (!res) {
         return 0;
      }
      res = CryptGenRandom(hCryptProv, (DWORD) sizeof(rng->seed), (PBYTE)rng->seed);
      CryptReleaseContext(hCryptProv, 0);
      if (!res) {
         return 0;
      }
      
#else
#error "unsupported platform"
      
#endif      
      return 1;
   }
   return 0;
}


static inline uint64_t
_rotl(const uint64_t x, int k) {
   return (x << k) | (x >> (64 - k));
}


uint64_t
prng_next(prng_t *rng) {
   if ( rng ) {
      const uint64_t s0 = rng->seed[0];
      uint64_t s1 = rng->seed[1];
      const uint64_t result = s0 + s1;

      s1 ^= s0;
      rng->seed[0] = _rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
      rng->seed[1] = _rotl(s1, 36); // c

      return result;
   }
   return 0;
}
