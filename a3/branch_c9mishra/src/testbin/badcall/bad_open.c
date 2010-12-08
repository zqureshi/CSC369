/*
 * invalid calls to open()
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
open_badflags(void)
{
	int fd;

	fd = open("null:", 309842);
	report_test(fd, errno, EINVAL, "open null: with bad flags");
}

static
void
open_empty(void)
{
	int rv;
	rv = open("", O_RDONLY);
	report_test2(rv, errno, 0, EINVAL, "open empty string");
	if (rv>=0) {
		close(rv);
	}
}

void
test_open(void)
{
	test_open_path();

	open_badflags();
	open_empty();
}
