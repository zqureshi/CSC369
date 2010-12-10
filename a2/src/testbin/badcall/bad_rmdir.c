/*
 * rmdir
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
rmdir_file(void)
{
	int rv;
	if (create_testfile()<0) {
		return;
	}
	rv = rmdir(TESTFILE);
	report_test(rv, errno, ENOTDIR, "rmdir a file");
	remove(TESTFILE);
}

static
void
rmdir_dot(void)
{
	int rv;

	rv = rmdir(".");
	report_test(rv, errno, EINVAL, "rmdir .");
}

static
void
rmdir_dotdot(void)
{
	int rv;

	rv = rmdir("..");
	report_test2(rv, errno, EINVAL, ENOTEMPTY, "rmdir ..");
}

static
void
rmdir_empty(void)
{
	int rv;
	rv = rmdir("");
	report_test(rv, errno, EINVAL, "rmdir empty string");
}

void
test_rmdir(void)
{
	test_rmdir_path();

	rmdir_file();
	rmdir_dot();
	rmdir_dotdot();
	rmdir_empty();
}
