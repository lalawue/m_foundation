/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef _CPRNG_H
#define _CPRNG_H

#ifdef __cplusplus
extern "C" {
#endif


// cryptographic pseudo-random number generator (CPRNG)
int cprng_random(unsigned char *buf, int buf_len);


#ifdef __cplusplus
}
#endif

#endif
