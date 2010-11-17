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
 * Standard C string function: append one string to another.
 */

char *
strcat(char *dest, const char *src)
{
	size_t offset = strlen(dest);
	strcpy(dest+offset, src);
	return dest;
}
