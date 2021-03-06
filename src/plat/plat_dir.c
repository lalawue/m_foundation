/* 
 * Copyright (c) 2015 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "plat_os.h"
#ifdef PLAT_OS_WIN
#include <direct.h> 
#include <io.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif
#include "m_mem.h"
#include "plat_dir.h"
#include "plat_charset.h"
#include <assert.h>
#include "mfoundation_import.h"

#if M_FOUNDATION_IMPORT_PLAT_DIR

static mdir_entry_t*
_create_dir_entry(int namelen) {
   if (namelen > 0) {
      mdir_entry_t *e = (mdir_entry_t*)mm_malloc(sizeof(*e) + namelen + 1);
      e->name = ((char*)e) + sizeof(*e);
      return e;
   }
   return NULL;
}

static void
_destroy_dir_entry(mdir_entry_t *d) {
   if ( d ) {
      mm_free(d);
   }
}

#ifdef PLAT_OS_WIN
typedef struct _stat stat_t;
#define S_IFMT _S_IFMT
#define S_IFDIR _S_IFDIR
#define S_IFREG _S_IFREG
#define sys_stat(A,B) _stat(A,B)
#else
typedef struct stat stat_t;
#define sys_stat(A,B) stat(A,B)
#endif

int
mdir_stat(const char *path, uint64_t *fsize, int *ftype) {
   stat_t st;
#ifdef PLAT_OS_WIN
   char tmp[MDIR_MAX_PATH] = {0}, fpath[MDIR_MAX_PATH] = {0};
   charset_to_sysm(path, tmp, MDIR_MAX_PATH, fpath, MDIR_MAX_PATH);
   path = (const char*)fpath;
#endif
   int ret = sys_stat(path, &st);
   if (ret == 0) {
      if (fsize) {
         *fsize = st.st_size;
      }
      if (ftype) {
         switch (st.st_mode & S_IFMT) {
            case S_IFDIR: *ftype = MDIR_DIRECOTRY; break;
            case S_IFREG: *ftype = MDIR_FILE; break;
            default: *ftype = MDIR_UNKNOW; break;
         }
      }
      return 1;
   }
   perror("lstat: ");
   return 0;
}

static void
_mdir_destroy_lst(lst_t *lst) {
   lst_foreach(n, lst) {
      mdir_entry_t *e = (mdir_entry_t*)lst_iter_data(n);
      _destroy_dir_entry(e);
   }
   lst_destroy(lst);
}

struct s_mdir {
#ifdef PLAT_OS_WIN
   int fh;
   struct _finddata_t fdata;
#else
   DIR *dirp;
   struct dirent *dp;
#endif
   lst_t *lst;
   char *dir_path;
   char *tmp_path;
};

mdir_t*
mdir_open(const char *dir_path) {
   if (dir_path == NULL) {
      return NULL;
   }

   mdir_t *d = NULL;
   do {
      d = (mdir_t*)mm_malloc(sizeof(*d));
      if (d == NULL) { break; };

#ifdef PLAT_OS_WIN
      char tmp[MDIR_MAX_PATH]={0}, fname[MDIR_MAX_PATH]={0};
      int len = charset_to_sysm(dir_path, tmp, MDIR_MAX_PATH, fname, MDIR_MAX_PATH);
      strcat(fname, "*.*");
      d->fh = _findfirst(fname, &d->fdata);
      if (d->fh < 0) { break; }
#else
      if ((d->dirp=opendir(dir_path)) == NULL) { break; }
#endif

      int dir_len = (int)strlen(dir_path);
      d->dir_path = (char*)mm_malloc(dir_len + 1);
      memcpy(d->dir_path, dir_path, dir_len);
      d->dir_path[dir_len] = 0;
      d->tmp_path = (char*)mm_malloc(2*MDIR_MAX_PATH);
      return d;
   } while (0);

   if (d) {
      mm_free(d);
   }
   return NULL;
}

static inline int
_is_current_dir_or_hide_file(char *dir) {
   return (dir[0]=='.' && (dir[1]!='.'));
}

lst_t*
mdir_list(mdir_t *d, int count) {
   int first_list = d->lst ? 0 : 1;
   if (d==NULL || count<=0 ) return NULL;
   if ( d->lst ) _mdir_destroy_lst(d->lst);

   lst_t *lst = lst_create();
#ifdef PLAT_OS_WIN   /* ignore cound */
   int fnext=1, i=0;
   char tmp[MDIR_MAX_PATH]={0}, fname[MDIR_MAX_PATH]={0};
   if ( !first_list ) { fnext=_findnext(d->fh, &d->fdata); i++; }
   for (i=0; (i<count) && fnext>=0; i++, fnext=_findnext(d->fh, &d->fdata)) {
      int namlen = charset_to_prog(d->fdata.name, tmp, MDIR_MAX_PATH, fname, MDIR_MAX_PATH) - 1;
      if ( _is_current_dir_or_hide_file(fname) ) { i--; }
      else if (d->fdata.attrib & (_A_HIDDEN | _A_SYSTEM)) { i--; }
      else {
         mdir_entry_t *e = _create_dir_entry(namlen + 1);
         if (d->fdata.attrib & _A_SUBDIR) { e->ftype = MDIR_DIRECOTRY; }
         else if (d->fdata.attrib & (_A_NORMAL | _A_RDONLY | _A_ARCH)) { e->ftype = MDIR_FILE; }
         else { e->ftype = MDIR_UNKNOW; }
         e->fsize = d->fdata.size;
         e->namlen = namlen;
         strncpy(e->name, fname, namlen);
         e->name[namlen] = 0;
         lst_pushl(lst, e);
      }
   }
#else
   struct dirent *dp = NULL;
   for (int i=0; (i<count) && (dp=readdir(d->dirp)); i++) {
      if ( _is_current_dir_or_hide_file(dp->d_name) ) { i--; }
      else {
         int ftype;
         uint64_t fsize;
         mdir_entry_t *e = NULL;

         sprintf(d->tmp_path, "%s/%s", d->dir_path, dp->d_name);
         if (mdir_stat(d->tmp_path, &fsize, &ftype) ) {
            int namlen = (int)strlen(dp->d_name);
            e = _create_dir_entry(namlen + 1);
            e->ftype = ftype;
            e->fsize = fsize;
            e->namlen = namlen;
            memcpy(e->name, dp->d_name, namlen);
            e->name[namlen] = 0;
            lst_pushl(lst, e);
         }
      }
   }
   (void)first_list;
   d->dp = dp;
#endif
   d->lst = lst;
   return lst;
}

void
mdir_close(mdir_t *d) {
   if ( d ) {
#ifdef PLAT_OS_WIN
      _findclose(d->fh);
#else
      closedir(d->dirp);
#endif
      if (d->lst) {
         _mdir_destroy_lst(d->lst);
      }
      mm_free(d->dir_path);
      mm_free(d->tmp_path);
      mm_free(d);
   }
}

char* 
mdir_getcwd(char *path_out, int out_len) {
   char *path = NULL;
#ifdef PLAT_OS_WIN
   path = _getcwd(path_out, out_len);
#else
   path = getcwd(path_out, out_len);
#endif
   return mdir_path_slash(path);;
}

char* mdir_path_slash(char *path) {
#ifdef PLAT_OS_WIN
   if (path) {
      char *c = path;
      for (int i=0; c[i]; i++) {
         c[i] = (c[i] == '\\') ? '/' : c[i];
      }
   }
#endif
   return path;
}

#endif // M_FOUNDATION_IMPORT_PLAT_DIR
