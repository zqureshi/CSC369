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
 * C standard function - copy a block of memory, handling overlapping
 * regions correctly.
 */

void *
memmove(void *dst, const void *src, size_t len)
{
	size_t i;

	/*
	 * If the buffers don't overlap, it doesn't matter what direction
	 * we copy in. If they do, it does, so just assume they always do.
	 * We don't concern ourselves with the possibility that the region
	 * to copy might roll over across the top of memory, because it's
	 * not going to happen.
	 *
	 * If the destination is above the source, we have to copy
	 * back to front to avoid overwriting the data we want to
	 * copy.
	 *
	 *      dest:       dddddddd
	 *      src:    ssssssss   ^
	 *              |   ^  |___|
         *              |___|
	 *
	 * If the destination is below the source, we have to copy
	 * front to back.
	 *
	 *      dest:   dddddddd
	 *      src:    ^   ssssssss
	 *              |___|  ^   |
         *                     |___|
	 */

	if ((uintptr_t)dst < (uintptr_t)src) {
		/*
		 * As author/maintainer of libc, take advantage of the
		 * fact that we know memcpy copies forwards.
		 */
		return memcpy(dst, src, len);
	}

	/*
	 * Copy by words in the common case. Look in memcpy.c for more
	 * information.
	 */

	if ((uintptr_t)dst % sizeof(long) == 0 &&
	    (uintptr_t)src % sizeof(long) == 0 &&
	    len % sizeof(long) == 0) {

		long *d = dst;
		const long *s = src;

		/*
		 * The reason we copy index i-1 and test i>0 is that
		 * i is unsigned - so testing i>=0 doesn't work.
		 */

		for (i=len/sizeof(long); i>0; i--) {
			d[i-1] = s[i-1];
		}
	}
	else {
		char *d = dst;
		const char *s = src;

		for (i=len; i>0; i--) {
			d[i-1] = s[i-1];
		}
	}

	return dst;
}
