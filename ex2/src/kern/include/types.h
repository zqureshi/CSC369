#ifndef _TYPES_H_
#define _TYPES_H_

/*
 * The model for the include files in the kernel is as follows:
 *
 *     1. Every source file includes this file, <types.h>, first.
 *
 *     2. Every other header file may assume this file has been
 *        included, but should explicitly include any other headers
 *        it requires to compile.
 */


/* Get machine-dependent types visible to userland */
#include <machine/types.h>

/* Get machine-independent types visible to userland */
#include <kern/types.h>

/* Get machine-dependent types not visible to userland */
#include <machine/ktypes.h>

/*
 * Define userptr_t as a pointer to a one-byte struct, so it won't mix
 * with other pointers.
 */

struct __userptr { char _dummy; };
typedef struct __userptr *userptr_t;
typedef const struct __userptr *const_userptr_t;

#endif /* _TYPES_H_ */
