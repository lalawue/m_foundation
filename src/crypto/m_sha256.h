/*
*   SHA-256 implementation, Mark 2
*
*   Copyright (c) 2010,2014 Ilya O. Levin, http://www.literatecode.com
*
*   Permission to use, copy, modify, and distribute this software for any
*   purpose with or without fee is hereby granted, provided that the above
*   copyright notice and this permission notice appear in all copies.
*
*   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
*   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
*   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
*   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
*   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
*   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
*   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
#ifndef SHA256_H_
#define SHA256_H_

#include <stddef.h>
#include "plat_os.h"

#ifdef PLAT_OS_WIN
#ifndef uint8_t
typedef unsigned __int8 uint8_t;
#endif
#ifndef uint32_t
typedef unsigned __int32 uint32_t;
#endif
#else
#include <stdint.h>
#endif

typedef struct s_sha256_context sha256_context_t;

#define SHA256_HASH_BYTES    32

#ifdef __cplusplus
extern "C" {
#endif

void sha256_init(sha256_context_t *ctx);
void sha256_hash(sha256_context_t *ctx, const void *data, size_t len);
void sha256_done(sha256_context_t *ctx, uint8_t *hash);

void sha256_once(const void *data, size_t len, uint8_t *hash);

#ifdef __cplusplus
}
#endif

#endif
