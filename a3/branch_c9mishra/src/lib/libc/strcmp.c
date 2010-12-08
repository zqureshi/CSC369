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
 * Standard C string function: compare two strings and return their
 * sort order.
 */

int
strcmp(const char *a, const char *b)
{
	size_t i;

	/*
	 * Walk down both strings until either they're different
	 * or we hit the end of A.
	 *
	 * If A and B strings are not the same length, when the
	 * shorter one ends, the two will be different, and we'll
	 * stop before running off the end of either.
	 *
	 * If they *are* the same length, it's sufficient to check
	 * that we haven't run off the end of A, because that's the
	 * same as checking to make sure we haven't run off the end of
	 * B.
	 */

	for (i=0; a[i]!=0 && a[i]==b[i]; i++);

	/*
	 * If A is greater than B, return 1. If A is less than B,
	 * return -1.  If they're the same, return 0. Since we have
	 * stopped at the first character of difference (or the end of
	 * both strings) checking the character under I accomplishes
	 * this.
	 *
	 * If we were doing internationalization, and/or we wanted to
	 * be able to sort accented characters in with normal text, a
	 * simple comparison of the values of the characters wouldn't
	 * be sufficient. But for present purposes, it's adequate.
	 */
	if (a[i]>b[i]) {
		return 1;
	}
	else if (a[i]==b[i]) {
		return 0;
	}
	return -1;
}
