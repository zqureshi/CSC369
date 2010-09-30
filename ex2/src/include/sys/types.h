#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_

/*
 * This header file is supposed to define standard system types,
 * stuff like size_t and pid_t, as well as define a few other
 * standard symbols like NULL.
 *
 * There are no such types that are user-level only, so we just
 * get the definitions that the kernel exports.
 *
 * <machine/types.h> gets us the machine-dependent types from the
 * kernel, and <kern/types.h> gets us the machine-independent types
 * from the kernel.
 */

#include <machine/types.h>
#include <kern/types.h>

#endif /* _SYS_TYPES_H_ */
