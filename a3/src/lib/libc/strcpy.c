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
 * Standard C string function: copy one string to another.
 */
char *
strcpy(char *dest, const char *src)
{
	size_t i;

	/*
	 * Copy characters until we hit the null terminator.
	 */
	for (i=0; src[i]; i++) {
		dest[i] = src[i];
	}

	/*
	 * Add null terminator to result.
	 */
	dest[i] = 0;

	return dest;
}
