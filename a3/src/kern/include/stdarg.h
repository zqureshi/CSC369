#ifndef _STDARG_H_
#define _STDARG_H_

/* Make sure we have __PF() */
#include <lib.h>

/*
 * As of gcc 3.0, the stdarg declarations can be made
 * machine-independent because gcc abstracts the implementations away
 * for us.
 */

typedef __builtin_va_list __va_list;
typedef __va_list va_list;

#define va_start(ap,fmt) __builtin_stdarg_start(ap,fmt)
#define va_arg(ap,t) __builtin_va_arg(ap,t)
#define va_copy(ap1,ap2) __builtin_va_copy(ap1,ap2)
#define va_end(ap) __builtin_va_end(ap)

/*
 * The v... versions of printf functions in <lib.h>. This is not really
 * the best place for them...
 */
void vkprintf(const char *fmt, va_list ap) __PF(1,0);
int vsnprintf(char *buf, size_t maxlen, const char *f, va_list ap) __PF(3,0);

/*
 * The printf driver function (shared with libc).
 * Does v...printf, passing the output data piecemeal to the function
 * supplied. The "clientdata" argument is passed through to the function.
 * The strings passed to the function might not be null-terminated.
 */
int __vprintf(void (*func)(void *clientdata, const char *str, size_t len), 
	      void *clientdata, const char *format, va_list ap) __PF(3,0);

#endif /* _STDARG_H_ */
