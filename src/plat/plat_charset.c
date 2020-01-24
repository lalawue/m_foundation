/* 
 * Copyright (c) 2015 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include "plat_os.h"
#ifdef PLAT_OS_WIN
#include <windows.h>
#include <direct.h> 
#endif
#include "mfoundation_import.h"

#if M_FOUNDATION_IMPORT_PLAT_CHARSET

int
charset_to_prog(
   const char *from, char *tmp, int tmplen, char *to, int tolen) {
#ifdef PLAT_OS_WIN
   int len = MultiByteToWideChar(CP_OEMCP, 0, (LPCCH)from, -1, (LPWSTR)tmp, tmplen);
   return WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)tmp, len, to, tolen, NULL, NULL);
#else
   return 0;
#endif
}

/* to system local */
int
charset_to_sysm(
   const char *from, char *tmp, int tmplen, char *to, int tolen) {
#ifdef PLAT_OS_WIN
   int len = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)from, -1, (LPWSTR)tmp, tmplen);
   return WideCharToMultiByte(CP_OEMCP, 0, (LPCWCH)tmp, len, to, tolen, NULL, NULL);
#else
   return 0;
#endif
}

#endif  /* M_FOUNDATION_IMPORT_PLAT_CHARSET */
