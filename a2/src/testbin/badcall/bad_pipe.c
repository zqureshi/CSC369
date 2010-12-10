/*
 * pipe
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

#include "config.h"
#include "test.h"

static
void
pipe_badptr(void *ptr, const char *desc)
{
	int rv;

	rv = pipe(ptr);
	report_test(rv, errno, EFAULT, desc);
}

static
void
pipe_unaligned(void)
{
	int fds[3], rv;
	char *ptr;

	ptr = (char *)&fds[0];
	ptr++;

	rv = pipe((int *)ptr);
	report_survival(rv, errno, "pipe with unaligned pointer");
}

void
test_pipe(void)
{
	pipe_badptr(NULL, "pipe with NULL pointer");
	pipe_badptr(INVAL_PTR, "pipe with invalid pointer");
	pipe_badptr(KERN_PTR, "pipe with kernel pointer");

	pipe_unaligned();
}
