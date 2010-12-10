/*
 * This file is shared between libc and the kernel, so don't put anything
 * in here that won't work in both contexts.
 */

#ifdef _KERNEL
#include <types.h>
#include <lib.h>
#else
#include <string.h>
#endif

/*
 * Standard (well, semi-standard) C string function - zero a block of
 * memory.
 */

void
bzero(void *vblock, size_t len)
{
	char *block = vblock;
	size_t i;

	/*
	 * For performance, optimize the common case where the pointer
	 * and the length are word-aligned, and write word-at-a-time
	 * instead of byte-at-a-time. Otherwise, write bytes.
	 *
	 * The alignment logic here should be portable. We rely on the
	 * compiler to be reasonably intelligent about optimizing the
	 * divides and moduli out. Fortunately, it is.
	 */

	if ((uintptr_t)block % sizeof(long) == 0 &&
	    len % sizeof(long) == 0) {
		long *lb = (long *)block;
		for (i=0; i<len/sizeof(long); i++) {
			lb[i] = 0;
		}
	}
	else {
		for (i=0; i<len; i++) {
			block[i] = 0;
		}
	}
}
