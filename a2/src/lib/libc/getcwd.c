#include <unistd.h>
#include <errno.h>

/*
 * POSIX C function: retrieve current working directory.
 * Uses the system call __getcwd(), which does essentially
 * all the work.
 */

char *
getcwd(char *buf, size_t buflen)
{
	int r;

	if (buflen < 1) {
		errno = EINVAL;
		return NULL;
	}

	r = __getcwd(buf, buflen-1);
	if (r < 0) {
		return NULL;
	}

	buf[r] = 0;
	return buf;
}
