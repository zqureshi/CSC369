/*
 * This file is shared between libc and the kernel, so don't put anything
 * in here that won't work in both contexts.
 */

#ifdef _KERNEL
#include <types.h>
#include <lib.h>

#else
#include <stdio.h>

#endif /* _KERNEL */

#include <stdarg.h>

/*
 * Standard C string/IO function: printf into a character buffer.
 */


/*
 * Context structure for snprintf: buffer to print into, maximum 
 * length, and index of the next character to write.
 *
 * Note that while the length argument to snprintf includes space for
 * a null terminator, SNP.buflen does not. This is to make something
 * vaguely reasonable happen if a length of 0 is passed to snprintf.
 */

typedef struct {
	char *buf;
	size_t buflen;
	size_t bufpos;
} SNP;

/*
 * Send function for snprintf. This is the function handed to the
 * printf guts. It gets called with mydata pointing to the context,
 * and some string data of length LEN in DATA. DATA is not necessarily
 * null-terminated.
 */

static
void
__snprintf_send(void *mydata, const char *data, size_t len)
{
	SNP *snp = mydata;
	unsigned i;

	/* For each character we're sent... */
	for (i=0; i<len; i++) {

		/* If we aren't past the length, */
		if (snp->bufpos < snp->buflen) {

			/* store the character */
			snp->buf[snp->bufpos] = data[i];

			/* and increment the position. */
			snp->bufpos++;
		}
	}
}

/*
 * The va_list version of snprintf.
 */
int
vsnprintf(char *buf, size_t len, const char *fmt, va_list ap)
{
	int chars;
	SNP snp;

	/*
	 * Fill in the context structure.
	 * We set snp.buflen to the number of characters that can be
	 * written (excluding the null terminator) so as not to have
	 * to special-case the possibility that we got passed a length
	 * of zero elsewhere.
	 */
	snp.buf = buf;
	if (len==0) {
		snp.buflen = 0;
	}
	else {
		snp.buflen = len-1;
	}
	snp.bufpos = 0;

	/* Call __vprintf to do the actual work. */
	chars = __vprintf(__snprintf_send, &snp, fmt, ap);

	/*
	 * Add a null terminator. If the length *we were passed* is greater
	 * than zero, we reserved a space in the buffer for the terminator,
	 * so this won't overflow. If the length we were passed is zero,
	 * nothing will have been or should be written anyway, and buf
	 * might even be NULL. (C99 explicitly endorses this possibility.)
	 */
	if (len > 0) {
		buf[snp.bufpos] = 0;
	}

	/*
	 * Return the number of characters __vprintf processed.
	 * According to C99, snprintf should return this number, not
	 * the number of characters actually stored, and should not
	 * return -1 on overflow but only on other errors.
	 */
	return chars;
}

/*
 * snprintf - hand off to vsnprintf.
 */
int
snprintf(char *buf, size_t len, const char *fmt, ...)
{
	int chars;
	va_list ap;
	va_start(ap, fmt);
	chars = vsnprintf(buf, len, fmt, ap);
	va_end(ap);
	return chars;
}

