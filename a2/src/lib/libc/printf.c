#include <stdio.h>
#include <stdarg.h>

/*
 * printf - C standard I/O function.
 */


/*
 * Function passed to __vprintf to do the actual output.
 */
static
void
__printf_send(void *mydata, const char *data, size_t len)
{
	unsigned i;
	(void)mydata;  /* not needed */

	for (i=0; i<len; i++) {
		putchar(data[i]);
	}
}

/* printf: hand off to vprintf */
int
printf(const char *fmt, ...)
{
	int chars;
	va_list ap;
	va_start(ap, fmt);
	chars = vprintf(fmt, ap);
	va_end(ap);
	return chars;
}

/* vprintf: call __vprintf to do the work. */
int
vprintf(const char *fmt, va_list ap)
{
	return __vprintf(__printf_send, NULL, fmt, ap);
}
