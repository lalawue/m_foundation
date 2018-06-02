/* 
 * Copyright (c) 2015 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */
#define _CRT_SECURE_NO_WARNINGS

#include "plat_os.h"
#ifdef PLAT_OS_WIN
#include <windows.h>
#include <stdint.h>
#else
#define _XOPEN_SOURCE 500
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "m_mem.h"
#include "utils_debug.h"
#include "mfoundation_import.h"

#if M_FOUNDATION_IMPORT_UTILS_DEBUG

typedef struct {
   unsigned char init;
   unsigned level;
   unsigned option;
   FILE *fp;
} debug_t;

typedef struct {
   unsigned count;
   debug_t *dbg;                /* dbg instance */
} ins_t;

static ins_t g_ins;

static inline debug_t* _dbg(unsigned ins) {
   return ins < g_ins.count ? &g_ins.dbg[ins] : NULL;
}

void debug_init(unsigned total_instance) {
   if (g_ins.count <= 0) {
      g_ins.count = total_instance;
      g_ins.dbg = (debug_t*)mm_malloc(sizeof(debug_t) * total_instance);
      return;
   }
   assert(0);
}

void debug_fini(void) {
   if (g_ins.count > 0) {
      for (unsigned i=0; i<g_ins.count; i++) {
         debug_close(i);
      }
      g_ins.count = 0;
      mm_free( g_ins.dbg );
      g_ins.dbg = NULL;
      return;
   }
   assert(0);
}

void debug_open(unsigned ins, char *fname) {
   debug_t *d = _dbg(ins);
   if (fname && !d->init) {
      if (strcmp(fname, "stdout") == 0) {
         d->fp = stdout;
      }
      else if (strcmp(fname, "stderr") == 0) {
         d->fp = stderr;
      }
      else {
         d->fp = fopen(fname, "a");
         assert( d->fp );
      }
      d->option = D_OPT_DEFAULT;
      d->level = D_VERBOSE;
      fprintf(d->fp, "### debug %d open (%s)\n", ins, fname);
      d->init = 1;
      return;
   }
   assert(0);
}

void debug_close(unsigned ins) {
   debug_t *d = _dbg(ins);
   if ( d->init ) {
      fprintf(d->fp, "### debug %d, close\n", ins);
      if ( d->fp ) {
         if (d->fp!=stdout && d->fp!=stderr) {
            fclose(d->fp);
         }
      }
      d->init = 0;
   }
}

void debug_set_option(unsigned ins, unsigned opt) {
   debug_t *d = _dbg(ins);
   if ( d->init ) {
      d->option = opt;
      fprintf(d->fp, "### debug %d option (0x%x)\n", ins, opt);
      return;
   }
   assert(0);
}

void debug_set_level(unsigned ins, unsigned level) {
   debug_t *d = _dbg(ins);
   if ( d->init ) {
      d->level = level;
      fprintf(d->fp, "### debug %d level (%d)\n", ins, level);
      return;
   }
   assert(0);
}

void debug_raw(unsigned ins, const char *fmt, ...) {
   debug_t *d = _dbg(ins);
   if ( d->init ) {
      va_list ap;
      va_start(ap, fmt);
      vfprintf(d->fp, fmt, ap);
      va_end(ap);

      if (d->option & D_OPT_FLUSH) {
         fflush(d->fp);
      }
   }
}

/* micro second */
int64_t debug_time(void) {
#ifdef PLAT_OS_WIN
   FILETIME ft;
   int64_t t;
   GetSystemTimeAsFileTime(&ft);
   t = (int64_t)ft.dwHighDateTime << 32 | ft.dwLowDateTime;
   return t / 10 - 11644473600000000; /* Jan 1, 1601 */
#else
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;
#endif
}

void
debug_log(unsigned ins,
          const char *mod,
          unsigned level,
          const char *fname,
          int line,
          const char *fmt, ...)
{
   debug_t *d = _dbg(ins);

   if ( d->init ) {

      if (level > d->level) {
         return;
      }

      if (d->option & D_OPT_LEVEL) {
         static const char *clev[D_VERBOSE + 1] = {
            "Err", "Warn", "Info", "Verbose"
         };
         level = level & D_VERBOSE;
         fprintf(d->fp, "%s) ", clev[level]);
      }

      if (d->option & D_OPT_TIME) {
#ifdef PLAT_OS_WIN
         int64_t tm = debug_time();
         fprintf(d->fp, "%u> ", tm);
#else
         struct tm stm; time_t tloc; struct timeval tv;
         time(&tloc);
         localtime_r(&tloc, &stm);
         gettimeofday(&tv, NULL);
         fprintf(d->fp, "%d/%d %02d:%02d:%02d.%03d> ",
                 stm.tm_mon+1, stm.tm_mday, stm.tm_hour,
                 stm.tm_min, stm.tm_sec, (int)tv.tv_usec>>10);
#endif
      }

      if ((d->option & D_OPT_FILE) && fname) {
         char *p = strrchr((char*)fname, '/');
         if ( p ) {
            fprintf(d->fp, "(%s:%d) ", p+1, line);
         } else {
            fprintf(d->fp, "(%s:%d) ", fname, line);
         }
      }

      fprintf(d->fp, "[%s] ", mod);

      va_list ap;
      va_start(ap, fmt);
      vfprintf(d->fp, fmt, ap);
      va_end(ap);

      if (d->option & D_OPT_FLUSH) {
         fflush(d->fp);
      }
   }
}

#endif  // M_FOUNDATION_IMPORT_UTILS_DEBUG
