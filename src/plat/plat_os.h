/* 
 * Copyright (c) 2015 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifdef __APPLE__
#import "TargetConditionals.h"

#if TARGET_OS_IPHONE
#define PLAT_OS_IOS 1

#elif TARGET_OS_MAC
#define PLAT_OS_MAC 1

#endif  /* __APPLE__ */

#elif defined(_WIN32) || defined(_WIN64)
#define PLAT_OS_WIN 1

#elif defined(__linux__)
#define PLAT_OS_LINUX 1

#elif defined(__FreeBSD__)
#define PLAT_OS_FREEBSD 1

#endif

