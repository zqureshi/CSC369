#ifndef _ERR_H_
#define _ERR_H_

#include <stdarg.h>

/*
 * 4.4BSD error-printing functions.
 *
 * These print the program name and the supplied message, and
 * (non-*x versions only) the string for the error currently
 * stored in "errno", and a newline. The err* versions then
 * exit with the supplied exitcode.
 *
 * The v* versions are to the non-v* versions like vprintf is to
 * printf.
 */

void warn(const char *fmt, ...);
void warnx(const char *fmt, ...);
void err(int exitcode, const char *fmt, ...);
void errx(int exitcode, const char *fmt, ...);

void vwarn(const char *fmt, va_list);
void vwarnx(const char *fmt, va_list);
void verr(int exitcode, const char *fmt, va_list);
void verrx(int exitcode, const char *fmt, va_list);

#endif /* _ERR_H_ */
