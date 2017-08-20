/* 
 * Copyright (c) 2015 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef UTILS_CONF_H
#define UTILS_CONF_H

#include "m_list.h"
#include "utils_str.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct s_conf conf_t;

conf_t* utils_conf_open(const char *conf_file);
void utils_conf_close(conf_t*);

str_t* utils_conf_value(conf_t*, const char *key);

#ifdef __cplusplus
}
#endif

#endif
