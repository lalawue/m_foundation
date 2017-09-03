/* 
 * Copyright (c) 2017 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef _XOR64_H
#define _XOR64_H

#include <stdint.h>

#define XOR64_CHECKSUM_SIZE 8 /* bytes */

int xor64_checksum_gen(uint8_t *buf, int len, uint8_t *checksum);
int xor64_checksum_check(uint8_t *buf, int len, uint8_t *checksum);

#endif
