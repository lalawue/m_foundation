/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef UTILS_ARGS_H
#define UTILS_ARGS_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct s_args args_t;

args_t* utils_args_open(int argc, const char *argv[]);

void utils_args_close(args_t*);

long
utils_args_integer(args_t*, const char *param);
   
const char*
utils_args_string(args_t*, const char *param);


#ifdef __cplusplus
}
#endif

#endif
