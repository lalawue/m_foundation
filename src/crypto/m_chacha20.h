/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

/* refers to 'https://cr.yp.to/chacha.html' */

#ifndef _CHACHA20_H
#define _CHACHA20_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
   uint64_t bytes;              /* record nonce bytes */
   unsigned char iv[8];         /* 64 bits */
   unsigned char core[64];      /* 256 bits */
} chacha20_ctx_t;

void chacha20_ctx_init(chacha20_ctx_t *ctx);

void chacha20_key_setup(chacha20_ctx_t *ctx, const void *key, int bytes); // max 32 bytes
void chacha20_iv_setup(chacha20_ctx_t *ctx, const void *iv, int bytes); // max 8 bytes

void chacha20_xor(chacha20_ctx_t *ctx, const void *in, void *out, int bytes);   


#ifdef __cplusplus
}
#endif

#endif  /* _CHACHA20_H */
