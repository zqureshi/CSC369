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
 * C standard string function: get length of a string
 */

size_t
strlen(const char *s)
{
	size_t ret = 0;
	while (s[ret]) {
		ret++;
	}
	return ret;
}
