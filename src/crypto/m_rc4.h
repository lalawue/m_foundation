/* 
 * Copyright (c) 2017 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef M_RC4_H
#define M_RC4_H

#ifdef __cplusplus
extern "C" {
#endif


#include <time.h>
#include <stdint.h>

#define RC4_CRYPTO_OCCUPY 8

enum {
   RC4_ERR_INSUFFICIENT_IN_BUFFER_SIZE = -1,
   RC4_ERR_INSUFFICIENT_OUT_BUFFER_SIZE = -2,
   RC4_ERR_OVER_TIME_SECOND = -3,
   RC4_ERR_KEY_INVALID = -4,   
};

uint64_t rc4_hash_key(const char * str, int sz);

int rc4_encrypt(const char *in, int in_sz, char *out, int out_sz, uint64_t key, time_t ti);
int rc4_decrypt(const char *in, int in_sz, char *out, int out_sz, uint64_t key, time_t ti);


#ifdef __cplusplus
}
#endif

#endif
