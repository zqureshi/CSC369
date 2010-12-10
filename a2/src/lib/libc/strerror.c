#include <string.h>
#include <kern/errmsg.h>

/*
 * Standard C function to return a string for a given errno.
 */
const char *
strerror(int errcode)
{
	if (errcode>=0 && errcode < sys_nerr) {
		return sys_errlist[errcode];
	}
	return "Unknown error number";
}
