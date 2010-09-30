/*
 * symlink
 */

#include <unistd.h>
#include <errno.h>

#include "test.h"

static
void
symlink_empty1(void)
{
	int rv;
	rv = symlink("", TESTLINK);
	report_test2(rv, errno, 0, EINVAL, "symlink -> empty string");
	remove(TESTLINK);
}

static
void
symlink_empty2(void)
{
	int rv;
	rv = symlink("foo", "");
	report_test(rv, errno, EINVAL, "symlink named empty string");
}

void
test_symlink(void)
{
	test_symlink_paths();
	symlink_empty1();
	symlink_empty2();
}
