/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "m_mem.h"
#include "m_list.h"
#include "utils_args.h"

struct s_args {
   int count;
   const char **params;
};


args_t*
utils_args_open(int argc, const char *argv[]) {
   args_t *args = NULL;   
   if (argc>1 && argv) {
      args = (args_t*)mm_malloc(sizeof(*args));
      if (args) {
         args->count = argc;
         args->params = argv;
      }
   }
   return args;
}

void
utils_args_close(args_t *args) {
   if (args) {
      mm_free(args);
   }
}

static int
_find_args(args_t *args, const char *param) {
   unsigned long len = strlen(param);   
   for (int i=1; i<args->count; i++) {
      if (strncmp(args->params[i], param, len) == 0 && (i+1)<args->count) {
         return i + 1;
      }
   }
   return -1;
}

long
utils_args_integer(args_t *args, const char *param) {
   if (args && param) {
      int index = _find_args(args, param);
      if (index >= 0) {
         return atol(args->params[index]);
      }
   }
   return UTILS_ARGS_INVALID_INTEGER; /* caution */
}

const char*
utils_args_string(args_t *args, const char *param) {
   if (args && param) {
      int index = _find_args(args, param);
      if (index >= 0) {
         return args->params[index];
      }
   }
   return NULL;
}
