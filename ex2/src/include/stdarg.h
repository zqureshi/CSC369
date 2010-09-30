#ifndef _STDARG_H_
#define _STDARG_H_

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

#endif /* _STDARG_H_ */
