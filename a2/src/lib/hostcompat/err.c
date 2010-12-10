/*
 * 4.4BSD error printing functions.
 */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "host-err.h"

#ifdef NEED_ERR

/*
 * This is initialized by hostcompat_init
 */
extern const char *hostcompat_progname;

/*
 * Common routine for all the *err* and *warn* functions.
 */
static
void
hostcompat_printerr(int use_errno, const char *fmt, va_list ap)
{
	const char *errmsg;

	/*
	 * Get the error message for the current errno.
	 * Do this early, before doing anything that might change the
	 * value in errno.
	 */
	errmsg = strerror(errno);

	/*
	 * Look up the program name.
	 * Strictly speaking we should pull off the rightmost
	 * path component of argv[0] and use that as the program
	 * name (this is how BSD err* prints) but it doesn't make
	 * much difference.
	 */
	if (hostcompat_progname != NULL) {
		fprintf(stderr, "%s: ", hostcompat_progname);
	}
	else {
		fprintf(stderr, "libhostcompat: hostcompat_init not called\n");
		fprintf(stderr, "libhostcompat-program: ");
	}

	/* process the printf format and args */
	vfprintf(stderr, fmt, ap);

	if (use_errno) {
		/* if we're using errno, print the error string from above. */
		fprintf(stderr, ": %s\n", errmsg);
	}
	else {
		/* otherwise, just a newline. */
		fprintf(stderr, "\n");
	}
}

/*
 * The va_list versions of the warn/err functions.
 */

/* warn/vwarn: use errno, don't exit */
void
vwarn(const char *fmt, va_list ap)
{
	hostcompat_printerr(1, fmt, ap);
}

/* warnx/vwarnx: don't use errno, don't exit */
void
vwarnx(const char *fmt, va_list ap)
{
	hostcompat_printerr(0, fmt, ap);
}

/* err/verr: use errno, then exit */
void
verr(int exitcode, const char *fmt, va_list ap)
{
	hostcompat_printerr(1, fmt, ap);
	exit(exitcode);
}

/* errx/verrx: don't use errno, but do then exit */
void
verrx(int exitcode, const char *fmt, va_list ap)
{
	hostcompat_printerr(0, fmt, ap);
	exit(exitcode);
}

/*
 * The non-va_list versions of the warn/err functions.
 * Just hand off to the va_list versions.
 */

void
warn(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vwarn(fmt, ap);
	va_end(ap);
}

void
warnx(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vwarnx(fmt, ap);
	va_end(ap);
}

void
err(int exitcode, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	verr(exitcode, fmt, ap);
	va_end(ap);
}

void
errx(int exitcode, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	verrx(exitcode, fmt, ap);
	va_end(ap);
}

#endif /* NEED_ERR */
