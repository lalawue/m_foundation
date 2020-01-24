/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

/* refers to 'https://cr.yp.to/chacha.html' */

#ifndef _CHACHA20_PRIV_H
#define _CHACHA20_PRIV_H

#include "mfoundation_import.h"

#if M_FOUNDATION_IMPORT_CRYPTO_CHACHA20

/* ecrypt-config.h */

/* *** Normally, it should not be necessary to edit this file. *** */

#ifndef ECRYPT_CONFIG
#define ECRYPT_CONFIG

/* ------------------------------------------------------------------------- */

/* Guess the endianness of the target architecture. */

/* 
 * The LITTLE endian machines:
 */
#if defined(__ultrix)           /* Older MIPS */
#define ECRYPT_LITTLE_ENDIAN
#elif defined(__alpha)          /* Alpha */
#define ECRYPT_LITTLE_ENDIAN
#elif defined(i386)             /* x86 (gcc) */
#define ECRYPT_LITTLE_ENDIAN
#elif defined(__i386)           /* x86 (gcc) */
#define ECRYPT_LITTLE_ENDIAN
#elif defined(_M_IX86)          /* x86 (MSC, Borland) */
#define ECRYPT_LITTLE_ENDIAN
#elif defined(_MSC_VER)         /* x86 (surely MSC) */
#define ECRYPT_LITTLE_ENDIAN
#elif defined(__INTEL_COMPILER) /* x86 (surely Intel compiler icl.exe) */
#define ECRYPT_LITTLE_ENDIAN

/* 
 * The BIG endian machines: 
 */
#elif defined(sun)              /* Newer Sparc's */
#define ECRYPT_BIG_ENDIAN
#elif defined(__ppc__)          /* PowerPC */
#define ECRYPT_BIG_ENDIAN

/* 
 * Finally machines with UNKNOWN endianness:
 */
#elif defined (_AIX)            /* RS6000 */
#define ECRYPT_UNKNOWN
#elif defined(__hpux)           /* HP-PA */
#define ECRYPT_UNKNOWN
#elif defined(__aux)            /* 68K */
#define ECRYPT_UNKNOWN
#elif defined(__dgux)           /* 88K (but P6 in latest boxes) */
#define ECRYPT_UNKNOWN
#elif defined(__sgi)            /* Newer MIPS */
#define ECRYPT_UNKNOWN
#else	                        /* Any other processor */
#define ECRYPT_UNKNOWN
#endif

/* ------------------------------------------------------------------------- */

/*
 * Find minimal-width types to store 8-bit, 16-bit, 32-bit, and 64-bit
 * integers.
 *
 * Note: to enable 64-bit types on 32-bit compilers, it might be
 * necessary to switch from ISO C90 mode to ISO C99 mode (e.g., gcc
 * -std=c99).
 */

#include <limits.h>

/* --- check char --- */

#if (UCHAR_MAX / 0xFU > 0xFU)
#ifndef I8T
#define I8T char
#define U8C(v) (v##U)

#if (UCHAR_MAX == 0xFFU)
#define ECRYPT_I8T_IS_BYTE
#endif

#endif

#if (UCHAR_MAX / 0xFFU > 0xFFU)
#ifndef I16T
#define I16T char
#define U16C(v) (v##U)
#endif

#if (UCHAR_MAX / 0xFFFFU > 0xFFFFU)
#ifndef I32T
#define I32T char
#define U32C(v) (v##U)
#endif

#if (UCHAR_MAX / 0xFFFFFFFFU > 0xFFFFFFFFU)
#ifndef I64T
#define I64T char
#define U64C(v) (v##U)
#define ECRYPT_NATIVE64
#endif

#endif
#endif
#endif
#endif

/* --- check short --- */

#if (USHRT_MAX / 0xFU > 0xFU)
#ifndef I8T
#define I8T short
#define U8C(v) (v##U)

#if (USHRT_MAX == 0xFFU)
#define ECRYPT_I8T_IS_BYTE
#endif

#endif

#if (USHRT_MAX / 0xFFU > 0xFFU)
#ifndef I16T
#define I16T short
#define U16C(v) (v##U)
#endif

#if (USHRT_MAX / 0xFFFFU > 0xFFFFU)
#ifndef I32T
#define I32T short
#define U32C(v) (v##U)
#endif

#if (USHRT_MAX / 0xFFFFFFFFU > 0xFFFFFFFFU)
#ifndef I64T
#define I64T short
#define U64C(v) (v##U)
#define ECRYPT_NATIVE64
#endif

#endif
#endif
#endif
#endif

/* --- check int --- */

#if (UINT_MAX / 0xFU > 0xFU)
#ifndef I8T
#define I8T int
#define U8C(v) (v##U)

#if (ULONG_MAX == 0xFFU)
#define ECRYPT_I8T_IS_BYTE
#endif

#endif

#if (UINT_MAX / 0xFFU > 0xFFU)
#ifndef I16T
#define I16T int
#define U16C(v) (v##U)
#endif

#if (UINT_MAX / 0xFFFFU > 0xFFFFU)
#ifndef I32T
#define I32T int
#define U32C(v) (v##U)
#endif

#if (UINT_MAX / 0xFFFFFFFFU > 0xFFFFFFFFU)
#ifndef I64T
#define I64T int
#define U64C(v) (v##U)
#define ECRYPT_NATIVE64
#endif

#endif
#endif
#endif
#endif

/* --- check long --- */

#if (ULONG_MAX / 0xFUL > 0xFUL)
#ifndef I8T
#define I8T long
#define U8C(v) (v##UL)

#if (ULONG_MAX == 0xFFUL)
#define ECRYPT_I8T_IS_BYTE
#endif

#endif

#if (ULONG_MAX / 0xFFUL > 0xFFUL)
#ifndef I16T
#define I16T long
#define U16C(v) (v##UL)
#endif

#if (ULONG_MAX / 0xFFFFUL > 0xFFFFUL)
#ifndef I32T
#define I32T long
#define U32C(v) (v##UL)
#endif

#if (ULONG_MAX / 0xFFFFFFFFUL > 0xFFFFFFFFUL)
#ifndef I64T
#define I64T long
#define U64C(v) (v##UL)
#define ECRYPT_NATIVE64
#endif

#endif
#endif
#endif
#endif

/* --- check long long --- */

#ifdef ULLONG_MAX

#if (ULLONG_MAX / 0xFULL > 0xFULL)
#ifndef I8T
#define I8T long long
#define U8C(v) (v##ULL)

#if (ULLONG_MAX == 0xFFULL)
#define ECRYPT_I8T_IS_BYTE
#endif

#endif

#if (ULLONG_MAX / 0xFFULL > 0xFFULL)
#ifndef I16T
#define I16T long long
#define U16C(v) (v##ULL)
#endif

#if (ULLONG_MAX / 0xFFFFULL > 0xFFFFULL)
#ifndef I32T
#define I32T long long
#define U32C(v) (v##ULL)
#endif

#if (ULLONG_MAX / 0xFFFFFFFFULL > 0xFFFFFFFFULL)
#ifndef I64T
#define I64T long long
#define U64C(v) (v##ULL)
#endif

#endif
#endif
#endif
#endif

#endif

/* --- check __int64 --- */

#ifdef _UI64_MAX

#if (_UI64_MAX / 0xFFFFFFFFULL > 0xFFFFFFFFULL)
#ifndef I64T
#define I64T __int64
#define U64C(v) (v##ui64)
#endif

#endif

#endif

/* ------------------------------------------------------------------------- */

#endif


/*****/

/* ecrypt-portable.h */

/*
 * WARNING: the conversions defined below are implemented as macros,
 * and should be used carefully. They should NOT be used with
 * parameters which perform some action. E.g., the following two lines
 * are not equivalent:
 * 
 *  1) ++x; y = ROTL32(x, n); 
 *  2) y = ROTL32(++x, n);
 */

/*
 * *** Please do not edit this file. ***
 *
 * The default macros can be overridden for specific architectures by
 * editing 'ecrypt-machine.h'.
 */

#ifndef ECRYPT_PORTABLE
#define ECRYPT_PORTABLE

//#include "ecrypt-config.h"

/* ------------------------------------------------------------------------- */

/*
 * The following types are defined (if available):
 *
 * u8:  unsigned integer type, at least 8 bits
 * u16: unsigned integer type, at least 16 bits
 * u32: unsigned integer type, at least 32 bits
 * u64: unsigned integer type, at least 64 bits
 *
 * s8, s16, s32, s64 -> signed counterparts of u8, u16, u32, u64
 *
 * The selection of minimum-width integer types is taken care of by
 * 'ecrypt-config.h'. Note: to enable 64-bit types on 32-bit
 * compilers, it might be necessary to switch from ISO C90 mode to ISO
 * C99 mode (e.g., gcc -std=c99).
 */

#ifdef I8T
typedef signed I8T s8;
typedef unsigned I8T u8;
#endif

#ifdef I16T
typedef signed I16T s16;
typedef unsigned I16T u16;
#endif

#ifdef I32T
typedef signed I32T s32;
typedef unsigned I32T u32;
#endif

#ifdef I64T
typedef signed I64T s64;
typedef unsigned I64T u64;
#endif

/*
 * The following macros are used to obtain exact-width results.
 */

#define U8V(v) ((u8)(v) & U8C(0xFF))
#define U16V(v) ((u16)(v) & U16C(0xFFFF))
#define U32V(v) ((u32)(v) & U32C(0xFFFFFFFF))
#define U64V(v) ((u64)(v) & U64C(0xFFFFFFFFFFFFFFFF))

/* ------------------------------------------------------------------------- */

/*
 * The following macros return words with their bits rotated over n
 * positions to the left/right.
 */

#define ECRYPT_DEFAULT_ROT

#define ROTL8(v, n) \
  (U8V((v) << (n)) | ((v) >> (8 - (n))))

#define ROTL16(v, n) \
  (U16V((v) << (n)) | ((v) >> (16 - (n))))

#define ROTL32(v, n) \
  (U32V((v) << (n)) | ((v) >> (32 - (n))))

#define ROTL64(v, n) \
  (U64V((v) << (n)) | ((v) >> (64 - (n))))

#define ROTR8(v, n) ROTL8(v, 8 - (n))
#define ROTR16(v, n) ROTL16(v, 16 - (n))
#define ROTR32(v, n) ROTL32(v, 32 - (n))
#define ROTR64(v, n) ROTL64(v, 64 - (n))

/* ------------------------------------------------------------------------- */

/*
 * The following macros return a word with bytes in reverse order.
 */

#define ECRYPT_DEFAULT_SWAP

#define SWAP16(v) \
  ROTL16(v, 8)

#define SWAP32(v) \
  ((ROTL32(v,  8) & U32C(0x00FF00FF)) | \
   (ROTL32(v, 24) & U32C(0xFF00FF00)))

#ifdef ECRYPT_NATIVE64
#define SWAP64(v) \
  ((ROTL64(v,  8) & U64C(0x000000FF000000FF)) | \
   (ROTL64(v, 24) & U64C(0x0000FF000000FF00)) | \
   (ROTL64(v, 40) & U64C(0x00FF000000FF0000)) | \
   (ROTL64(v, 56) & U64C(0xFF000000FF000000)))
#else
#define SWAP64(v) \
  (((u64)SWAP32(U32V(v)) << 32) | (u64)SWAP32(U32V(v >> 32)))
#endif

#define ECRYPT_DEFAULT_WTOW

#ifdef ECRYPT_LITTLE_ENDIAN
#define U16TO16_LITTLE(v) (v)
#define U32TO32_LITTLE(v) (v)
#define U64TO64_LITTLE(v) (v)

#define U16TO16_BIG(v) SWAP16(v)
#define U32TO32_BIG(v) SWAP32(v)
#define U64TO64_BIG(v) SWAP64(v)
#endif

#ifdef ECRYPT_BIG_ENDIAN
#define U16TO16_LITTLE(v) SWAP16(v)
#define U32TO32_LITTLE(v) SWAP32(v)
#define U64TO64_LITTLE(v) SWAP64(v)

#define U16TO16_BIG(v) (v)
#define U32TO32_BIG(v) (v)
#define U64TO64_BIG(v) (v)
#endif

/*
 * The following macros load words from an array of bytes with
 * different types of endianness, and vice versa.
 */

#define ECRYPT_DEFAULT_BTOW

#if (!defined(ECRYPT_UNKNOWN) && defined(ECRYPT_I8T_IS_BYTE))

#define U8TO16_LITTLE(p) U16TO16_LITTLE(((u16*)(p))[0])
#define U8TO32_LITTLE(p) U32TO32_LITTLE(((u32*)(p))[0])
#define U8TO64_LITTLE(p) U64TO64_LITTLE(((u64*)(p))[0])

#define U8TO16_BIG(p) U16TO16_BIG(((u16*)(p))[0])
#define U8TO32_BIG(p) U32TO32_BIG(((u32*)(p))[0])
#define U8TO64_BIG(p) U64TO64_BIG(((u64*)(p))[0])

#define U16TO8_LITTLE(p, v) (((u16*)(p))[0] = U16TO16_LITTLE(v))
#define U32TO8_LITTLE(p, v) (((u32*)(p))[0] = U32TO32_LITTLE(v))
#define U64TO8_LITTLE(p, v) (((u64*)(p))[0] = U64TO64_LITTLE(v))

#define U16TO8_BIG(p, v) (((u16*)(p))[0] = U16TO16_BIG(v))
#define U32TO8_BIG(p, v) (((u32*)(p))[0] = U32TO32_BIG(v))
#define U64TO8_BIG(p, v) (((u64*)(p))[0] = U64TO64_BIG(v))

#else

#define U8TO16_LITTLE(p) \
  (((u16)((p)[0])      ) | \
   ((u16)((p)[1]) <<  8))

#define U8TO32_LITTLE(p) \
  (((u32)((p)[0])      ) | \
   ((u32)((p)[1]) <<  8) | \
   ((u32)((p)[2]) << 16) | \
   ((u32)((p)[3]) << 24))

#ifdef ECRYPT_NATIVE64
#define U8TO64_LITTLE(p) \
  (((u64)((p)[0])      ) | \
   ((u64)((p)[1]) <<  8) | \
   ((u64)((p)[2]) << 16) | \
   ((u64)((p)[3]) << 24) | \
   ((u64)((p)[4]) << 32) | \
   ((u64)((p)[5]) << 40) | \
   ((u64)((p)[6]) << 48) | \
   ((u64)((p)[7]) << 56))
#else
#define U8TO64_LITTLE(p) \
  ((u64)U8TO32_LITTLE(p) | ((u64)U8TO32_LITTLE((p) + 4) << 32))
#endif

#define U8TO16_BIG(p) \
  (((u16)((p)[0]) <<  8) | \
   ((u16)((p)[1])      ))

#define U8TO32_BIG(p) \
  (((u32)((p)[0]) << 24) | \
   ((u32)((p)[1]) << 16) | \
   ((u32)((p)[2]) <<  8) | \
   ((u32)((p)[3])      ))

#ifdef ECRYPT_NATIVE64
#define U8TO64_BIG(p) \
  (((u64)((p)[0]) << 56) | \
   ((u64)((p)[1]) << 48) | \
   ((u64)((p)[2]) << 40) | \
   ((u64)((p)[3]) << 32) | \
   ((u64)((p)[4]) << 24) | \
   ((u64)((p)[5]) << 16) | \
   ((u64)((p)[6]) <<  8) | \
   ((u64)((p)[7])      ))
#else
#define U8TO64_BIG(p) \
  (((u64)U8TO32_BIG(p) << 32) | (u64)U8TO32_BIG((p) + 4))
#endif

#define U16TO8_LITTLE(p, v) \
  do { \
    (p)[0] = U8V((v)      ); \
    (p)[1] = U8V((v) >>  8); \
  } while (0)

#define U32TO8_LITTLE(p, v) \
  do { \
    (p)[0] = U8V((v)      ); \
    (p)[1] = U8V((v) >>  8); \
    (p)[2] = U8V((v) >> 16); \
    (p)[3] = U8V((v) >> 24); \
  } while (0)

#ifdef ECRYPT_NATIVE64
#define U64TO8_LITTLE(p, v) \
  do { \
    (p)[0] = U8V((v)      ); \
    (p)[1] = U8V((v) >>  8); \
    (p)[2] = U8V((v) >> 16); \
    (p)[3] = U8V((v) >> 24); \
    (p)[4] = U8V((v) >> 32); \
    (p)[5] = U8V((v) >> 40); \
    (p)[6] = U8V((v) >> 48); \
    (p)[7] = U8V((v) >> 56); \
  } while (0)
#else
#define U64TO8_LITTLE(p, v) \
  do { \
    U32TO8_LITTLE((p),     U32V((v)      )); \
    U32TO8_LITTLE((p) + 4, U32V((v) >> 32)); \
  } while (0)
#endif

#define U16TO8_BIG(p, v) \
  do { \
    (p)[0] = U8V((v)      ); \
    (p)[1] = U8V((v) >>  8); \
  } while (0)

#define U32TO8_BIG(p, v) \
  do { \
    (p)[0] = U8V((v) >> 24); \
    (p)[1] = U8V((v) >> 16); \
    (p)[2] = U8V((v) >>  8); \
    (p)[3] = U8V((v)      ); \
  } while (0)

#ifdef ECRYPT_NATIVE64
#define U64TO8_BIG(p, v) \
  do { \
    (p)[0] = U8V((v) >> 56); \
    (p)[1] = U8V((v) >> 48); \
    (p)[2] = U8V((v) >> 40); \
    (p)[3] = U8V((v) >> 32); \
    (p)[4] = U8V((v) >> 24); \
    (p)[5] = U8V((v) >> 16); \
    (p)[6] = U8V((v) >>  8); \
    (p)[7] = U8V((v)      ); \
  } while (0)
#else
#define U64TO8_BIG(p, v) \
  do { \
    U32TO8_BIG((p),     U32V((v) >> 32)); \
    U32TO8_BIG((p) + 4, U32V((v)      )); \
  } while (0)
#endif

#endif

/* ------------------------------------------------------------------------- */

#endif

/*****/

/* ecrypt-machine.h */

/*
 * This file is included by 'ecrypt-portable.h'. It allows to override
 * the default macros for specific platforms. Please carefully check
 * the machine code generated by your compiler (with optimisations
 * turned on) before deciding to edit this file.
 */

/* ------------------------------------------------------------------------- */

#if (defined(ECRYPT_DEFAULT_ROT) && !defined(ECRYPT_MACHINE_ROT))

#define ECRYPT_MACHINE_ROT

#if (defined(WIN32) && defined(_MSC_VER))

#undef ROTL32
#undef ROTR32
#undef ROTL64
#undef ROTR64

#include <stdlib.h>

#define ROTL32(v, n) _lrotl(v, n)
#define ROTR32(v, n) _lrotr(v, n)
#define ROTL64(v, n) _rotl64(v, n)
#define ROTR64(v, n) _rotr64(v, n)

#endif

#endif

/* ------------------------------------------------------------------------- */

#if (defined(ECRYPT_DEFAULT_SWAP) && !defined(ECRYPT_MACHINE_SWAP))

#define ECRYPT_MACHINE_SWAP

/*
 * If you want to overwrite the default swap macros, put it here. And so on.
 */

#endif

/* ------------------------------------------------------------------------- */


/*****/


/* ecrypt-sync.h */

/* 
 * Header file for synchronous stream ciphers without authentication
 * mechanism.
 * 
 * *** Please only edit parts marked with "[edit]". ***
 */

#ifndef ECRYPT_SYNC
#define ECRYPT_SYNC

/* ------------------------------------------------------------------------- */

/* Cipher parameters */

/* 
 * The name of your cipher.
 */
#define ECRYPT_NAME "ChaCha8"
#define ECRYPT_PROFILE "_____"

/*
 * Specify which key and IV sizes are supported by your cipher. A user
 * should be able to enumerate the supported sizes by running the
 * following code:
 *
 * for (i = 0; ECRYPT_KEYSIZE(i) <= ECRYPT_MAXKEYSIZE; ++i)
 *   {
 *     keysize = ECRYPT_KEYSIZE(i);
 *
 *     ...
 *   }
 *
 * All sizes are in bits.
 */

#define ECRYPT_MAXKEYSIZE 256                 /* [edit] */
#define ECRYPT_KEYSIZE(i) (128 + (i)*128)     /* [edit] */

#define ECRYPT_MAXIVSIZE 64                   /* [edit] */
#define ECRYPT_IVSIZE(i) (64 + (i)*64)        /* [edit] */

/* ------------------------------------------------------------------------- */

/* Data structures */

/* 
 * ECRYPT_ctx is the structure containing the representation of the
 * internal state of your cipher. 
 */

typedef struct
{
  u32 input[16]; /* could be compressed */
  /* 
   * [edit]
   *
   * Put here all state variable needed during the encryption process.
   */
} ECRYPT_ctx;

/* ------------------------------------------------------------------------- */

/* Mandatory functions */

/*
 * Key and message independent initialization. This function will be
 * called once when the program starts (e.g., to build expanded S-box
 * tables).
 */
void ECRYPT_init();

/*
 * Key setup. It is the user's responsibility to select the values of
 * keysize and ivsize from the set of supported values specified
 * above.
 */
void ECRYPT_keysetup(
  ECRYPT_ctx* ctx, 
  const u8* key, 
  u32 keysize,                /* Key size in bits. */ 
  u32 ivsize);                /* IV size in bits. */ 

/*
 * IV setup. After having called ECRYPT_keysetup(), the user is
 * allowed to call ECRYPT_ivsetup() different times in order to
 * encrypt/decrypt different messages with the same key but different
 * IV's.
 */
void ECRYPT_ivsetup(
  ECRYPT_ctx* ctx, 
  const u8* iv);

/*
 * Encryption/decryption of arbitrary length messages.
 *
 * For efficiency reasons, the API provides two types of
 * encrypt/decrypt functions. The ECRYPT_encrypt_bytes() function
 * (declared here) encrypts byte strings of arbitrary length, while
 * the ECRYPT_encrypt_blocks() function (defined later) only accepts
 * lengths which are multiples of ECRYPT_BLOCKLENGTH.
 * 
 * The user is allowed to make multiple calls to
 * ECRYPT_encrypt_blocks() to incrementally encrypt a long message,
 * but he is NOT allowed to make additional encryption calls once he
 * has called ECRYPT_encrypt_bytes() (unless he starts a new message
 * of course). For example, this sequence of calls is acceptable:
 *
 * ECRYPT_keysetup();
 *
 * ECRYPT_ivsetup();
 * ECRYPT_encrypt_blocks();
 * ECRYPT_encrypt_blocks();
 * ECRYPT_encrypt_bytes();
 *
 * ECRYPT_ivsetup();
 * ECRYPT_encrypt_blocks();
 * ECRYPT_encrypt_blocks();
 *
 * ECRYPT_ivsetup();
 * ECRYPT_encrypt_bytes();
 * 
 * The following sequence is not:
 *
 * ECRYPT_keysetup();
 * ECRYPT_ivsetup();
 * ECRYPT_encrypt_blocks();
 * ECRYPT_encrypt_bytes();
 * ECRYPT_encrypt_blocks();
 */

void ECRYPT_encrypt_bytes(
  ECRYPT_ctx* ctx, 
  const u8* plaintext, 
  u8* ciphertext, 
  u32 msglen);                /* Message length in bytes. */ 

void ECRYPT_decrypt_bytes(
  ECRYPT_ctx* ctx, 
  const u8* ciphertext, 
  u8* plaintext, 
  u32 msglen);                /* Message length in bytes. */ 

/* ------------------------------------------------------------------------- */

/* Optional features */

/* 
 * For testing purposes it can sometimes be useful to have a function
 * which immediately generates keystream without having to provide it
 * with a zero plaintext. If your cipher cannot provide this function
 * (e.g., because it is not strictly a synchronous cipher), please
 * reset the ECRYPT_GENERATES_KEYSTREAM flag.
 */

#define ECRYPT_GENERATES_KEYSTREAM
#ifdef ECRYPT_GENERATES_KEYSTREAM

void ECRYPT_keystream_bytes(
  ECRYPT_ctx* ctx,
  u8* keystream,
  u32 length);                /* Length of keystream in bytes. */

#endif

/* ------------------------------------------------------------------------- */

/* Optional optimizations */

/* 
 * By default, the functions in this section are implemented using
 * calls to functions declared above. However, you might want to
 * implement them differently for performance reasons.
 */

/*
 * All-in-one encryption/decryption of (short) packets.
 *
 * The default definitions of these functions can be found in
 * "ecrypt-sync.c". If you want to implement them differently, please
 * undef the ECRYPT_USES_DEFAULT_ALL_IN_ONE flag.
 */
#define ECRYPT_USES_DEFAULT_ALL_IN_ONE        /* [edit] */

void ECRYPT_encrypt_packet(
  ECRYPT_ctx* ctx, 
  const u8* iv,
  const u8* plaintext, 
  u8* ciphertext, 
  u32 msglen);

void ECRYPT_decrypt_packet(
  ECRYPT_ctx* ctx, 
  const u8* iv,
  const u8* ciphertext, 
  u8* plaintext, 
  u32 msglen);

/*
 * Encryption/decryption of blocks.
 * 
 * By default, these functions are defined as macros. If you want to
 * provide a different implementation, please undef the
 * ECRYPT_USES_DEFAULT_BLOCK_MACROS flag and implement the functions
 * declared below.
 */

#define ECRYPT_BLOCKLENGTH 64                  /* [edit] */

#define ECRYPT_USES_DEFAULT_BLOCK_MACROS      /* [edit] */
#ifdef ECRYPT_USES_DEFAULT_BLOCK_MACROS

#define ECRYPT_encrypt_blocks(ctx, plaintext, ciphertext, blocks)  \
  ECRYPT_encrypt_bytes(ctx, plaintext, ciphertext,                 \
    (blocks) * ECRYPT_BLOCKLENGTH)

#define ECRYPT_decrypt_blocks(ctx, ciphertext, plaintext, blocks)  \
  ECRYPT_decrypt_bytes(ctx, ciphertext, plaintext,                 \
    (blocks) * ECRYPT_BLOCKLENGTH)

#ifdef ECRYPT_GENERATES_KEYSTREAM

#define ECRYPT_keystream_blocks(ctx, keystream, blocks)            \
  ECRYPT_keystream_bytes(ctx, keystream,                        \
    (blocks) * ECRYPT_BLOCKLENGTH)

#endif

#else

void ECRYPT_encrypt_blocks(
  ECRYPT_ctx* ctx, 
  const u8* plaintext, 
  u8* ciphertext, 
  u32 blocks);                /* Message length in blocks. */ 

void ECRYPT_decrypt_blocks(
  ECRYPT_ctx* ctx, 
  const u8* ciphertext, 
  u8* plaintext, 
  u32 blocks);                /* Message length in blocks. */ 

#ifdef ECRYPT_GENERATES_KEYSTREAM

void ECRYPT_keystream_blocks(
  ECRYPT_ctx* ctx,
  const u8* keystream,
  u32 blocks);                /* Keystream length in blocks. */ 

#endif

#endif

/*
 * If your cipher can be implemented in different ways, you can use
 * the ECRYPT_VARIANT parameter to allow the user to choose between
 * them at compile time (e.g., gcc -DECRYPT_VARIANT=3 ...). Please
 * only use this possibility if you really think it could make a
 * significant difference and keep the number of variants
 * (ECRYPT_MAXVARIANT) as small as possible (definitely not more than
 * 10). Note also that all variants should have exactly the same
 * external interface (i.e., the same ECRYPT_BLOCKLENGTH, etc.). 
 */
#define ECRYPT_MAXVARIANT 1                   /* [edit] */

#ifndef ECRYPT_VARIANT
#define ECRYPT_VARIANT 1
#endif

#if (ECRYPT_VARIANT > ECRYPT_MAXVARIANT)
#error this variant does not exist
#endif

/* ------------------------------------------------------------------------- */

#endif

#endif /* M_FOUNDATION_IMPORT_CRYPTO_CHACHA20 */

#endif  /* _CHACHA20_PRIV_H */
