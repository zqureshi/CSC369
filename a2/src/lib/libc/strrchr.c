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
 * C standard string function: find rightmost instance of a character
 * in a string.
 */
char *
strrchr(const char *s, int ch)
{
	/* start one past the last character INCLUDING NULL TERMINATOR */
	unsigned i = strlen(s)+1;

	/* go from right to left; stop at 0 */
	while (i>0) {

		/* decrement first */
		i--;

		/* now check the character we're over */
		if (s[i]==ch) {
			return (char *)(s+i);
		}
	}

	/* didn't find it */
	return NULL;
}
