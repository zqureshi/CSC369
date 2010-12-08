/*
 * __time
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
time_badsecs(void *ptr, const char *desc)
{
	int rv;

	rv = __time(ptr, NULL);
	report_test(rv, errno, EFAULT, desc);
}

static
void
time_badnsecs(void *ptr, const char *desc)
{
	int rv;

	rv = __time(NULL, ptr);
	report_test(rv, errno, EFAULT, desc);
}

void
test_time(void)
{
	time_badsecs(INVAL_PTR, "__time with invalid seconds pointer");
	time_badsecs(KERN_PTR, "__time with kernel seconds pointer");

	time_badnsecs(INVAL_PTR, "__time with invalid nsecs pointer");
	time_badnsecs(KERN_PTR, "__time with kernel nsecs pointer");
}
