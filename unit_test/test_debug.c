/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifdef M_FOUNDATION_TEST_DEBUG

#include <stdio.h>
#include "utils_debug.h"

#define _err(...) do {                            \
      _mlog(0, "TEST", D_ERROR, __VA_ARGS__);     \
      _mlog(1, "TEST", D_ERROR, __VA_ARGS__);     \
   } while (0)

#define _warn(...) do {                          \
      _mlog(0, "TEST", D_WARN, __VA_ARGS__);     \
      _mlog(1, "TEST", D_WARN, __VA_ARGS__);     \
   } while (0)

#define _info(...) do {                          \
      _mlog(0, "TEST", D_INFO, __VA_ARGS__);     \
      _mlog(1, "TEST", D_INFO, __VA_ARGS__);     \
   } while (0)

#define _verbose(...) do {                             \
      _mlog(0, "TEST", D_VERBOSE, __VA_ARGS__);        \
      _mlog(1, "TEST", D_VERBOSE, __VA_ARGS__);        \
   } while (0)


int
main(int argc, char *argv[]) {
   
   if (argc < 2) {
      printf("%s OUTPUT_FILE_NAME\n", argv[0]);
      return 0;
   }
   
   unsigned ins;
   
   debug_init(2);

   ins = 0;
   debug_open(ins, "stdout");
   debug_set_option(ins, D_OPT_LEVEL | D_OPT_TIME);
   debug_set_level(ins, D_VERBOSE);

   ins = 1;
   debug_open(ins, argv[1]);
   debug_set_option(ins, D_OPT_TIME | D_OPT_FILE);
   debug_set_level(ins, D_INFO);

   _err("err log\n");
   _warn("warn log\n");      
   _info("info log\n");
   _verbose("verbose log\n");

   debug_fini();
   return 0;
}

#endif // M_FOUNDATION_TEST_DEBUG
