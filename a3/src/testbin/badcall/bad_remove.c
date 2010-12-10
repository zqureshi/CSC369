/*
 * remove
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
remove_dir(void)
{
	int rv;

	if (create_testdir() < 0) {
		return;
	}

	rv = remove(TESTDIR);
	report_test(rv, errno, EISDIR, "remove() on a directory");
	rmdir(TESTDIR);
}

static
void
remove_dot(void)
{
	int rv;
	rv = remove(".");
	report_test2(rv, errno, EISDIR, EINVAL, "remove() on .");
}

static
void
remove_dotdot(void)
{
	int rv;
	rv = remove("..");
	report_test2(rv, errno, EISDIR, EINVAL, "remove() on ..");
}

static
void
remove_empty(void)
{
	int rv;
	rv = remove("");
	report_test2(rv, errno, EISDIR, EINVAL, "remove() on empty string");
}

void
test_remove(void)
{
	test_remove_path();

	remove_dir();
	remove_dot();
	remove_dotdot();
	remove_empty();
}
