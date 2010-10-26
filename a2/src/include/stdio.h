#ifndef _STDIO_H_
#define _STDIO_H_

/*
 * According to ANSI C we are not supposed to do this, but we're not 
 * exactly ANSI-compliant anyway.
 */
#include <sys/types.h>
#include <stdarg.h>

/* Constant returned by a bunch of stdio functions on error */
#define EOF (-1)

/*
 * The actual guts of printf
 * (for libc internal use only)
 */
int __vprintf(void (*sendfunc)(void *clientdata, const char *, size_t len),
	      void *clientdata,
	      const char *fmt,
	      __va_list ap);

/* Printf calls for user programs */
int printf(const char *fmt, ...);
int vprintf(const char *fmt, __va_list ap);
int snprintf(char *buf, size_t len, const char *fmt, ...);
int vsnprintf(char *buf, size_t len, const char *fmt, __va_list ap);

/* Print the argument string and then a newline. Returns 0 or -1 on error. */
int puts(const char *);

/* Like puts, but without the newline. Returns #chars written. */
/* Nonstandard C, hence the __. */
int __puts(const char *);

/* Writes one character. Returns it. */
int putchar(int);

/* Reads one character (0-255) or returns EOF on error. */
int getchar(void);

#endif /* _STDIO_H_ */
