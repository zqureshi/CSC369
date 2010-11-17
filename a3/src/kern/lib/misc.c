#include <types.h>
#include <kern/errmsg.h>
#include <lib.h>

/*
 * Like strdup, but calls kmalloc.
 */
char *
kstrdup(const char *s)
{
	char *z = kmalloc(strlen(s)+1);
	if (z==NULL) {
		return NULL;
	}
	strcpy(z, s);
	return z;
}

/*
 * Standard C function to return a string for a given errno.
 * Kernel version; panics if it hits an unknown error.
 */
const char *
strerror(int errcode)
{
	if (errcode>=0 && errcode < sys_nerr) {
		return sys_errlist[errcode];
	}
	panic("Invalid error code %d\n", errcode);
	return NULL;
}
