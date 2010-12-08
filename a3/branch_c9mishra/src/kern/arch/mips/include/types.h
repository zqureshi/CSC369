#ifndef _MIPS_TYPES_H_
#define _MIPS_TYPES_H_

/*
 * Basic integer types, and some related definitions, for MIPS.
 * This file is made visible to userland.
 *
 * Note: while we define int64_t and u_int64_t, actually using them
 * will in some cases cause odd link errors. This has to do with the
 * way gcc implements 64-bit arithmetic on 32-bit processors. When you
 * build gcc, it creates a library called libgcc.a that contains,
 * among other things, implementations of the functions gcc expects to
 * be able to call for doing 64-bit arithmetic. Unfortunately, you
 * don't really want to use the standard libgcc.a in a kernel, and
 * because of the circumstances with the compiler and toolchain in
 * cs161 it's awkward to use libgcc.a in userlevel programs too, so we
 * just don't support it. So don't use 64-bit integers unless you're
 * prepared to cope with these issues yourself.
 */

typedef char      int8_t;		/* 8-bit signed integer */
typedef short     int16_t;		/* 16-bit signed integer */
typedef int       int32_t;		/* 32-bit signed integer */
typedef long long int64_t;		/* 64-bit signed integer */

typedef unsigned char      u_int8_t;	/* 8-bit unsigned integer */
typedef unsigned short     u_int16_t;	/* 16-bit unsigned integer */
typedef unsigned int       u_int32_t;	/* 32-bit unsigned integer */
typedef unsigned long long u_int64_t;	/* 64-bit unsigned integer */

/*
 * Since we're a 32-bit platform, size_t can correctly be either
 * unsigned int or unsigned long. However, if we don't define it to
 * the same one gcc is using, gcc will get upset. If you switch
 * compilers and see otherwise unexplicable type errors involving
 * size_t, try changing this.
 */
#if 1
typedef unsigned size_t;		/* Size of a memory region */
#else
typedef unsigned long size_t;		/* Size of a memory region */
#endif

typedef long intptr_t;			/* Signed pointer-sized integer */
typedef unsigned long uintptr_t;	/* Unsigned pointer-sized integer */

/*
 * Number of bits per byte.
 */

#define CHAR_BIT  8

/*
 * Null pointer.
 */

#undef NULL
#define NULL ((void *)0)

/*
 * Endianness. While the MIPS can be either big-endian (mipseb) or
 * little-endian (mipsel), at least for now we only do mipseb.
 */

#undef _LITTLE_ENDIAN
#define _BIG_ENDIAN

#endif /* _MIPS_TYPES_H_ */
