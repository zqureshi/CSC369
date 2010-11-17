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
 * C standard string function: find leftmost instance of a character
 * in a string.
 */
char *
strchr(const char *s, int ch)
{
	/* scan from left to right */
	while (*s) {
		/* if we hit it, return it */
		if (*s==ch) {
			return (char *)s;
		}
		s++;
	}

	/* if we were looking for the 0, return that */
	if (*s==ch) {
		return (char *)s;
	}

	/* didn't find it */
	return NULL;
}
