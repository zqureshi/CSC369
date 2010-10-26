#include <stdio.h>

/*
 * Nonstandard (hence the __) version of puts that doesn't append
 * a newline.
 *
 * Returns the length of the string printed.
 */

int
__puts(const char *str)
{
	int count=0;
	while (*str) {
		putchar(*str);
		str++;
		count++;
	}
	return count;
}
