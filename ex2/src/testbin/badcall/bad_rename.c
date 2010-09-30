/*
 * rename
 */

#include <unistd.h>
#include <errno.h>

#include "test.h"

static
void
rename_dot(void)
{
	int rv;
	rv = rename(".", TESTDIR);
	report_test(rv, errno, EINVAL, "rename .");
	if (rv==0) {
		/* oops... put it back */
		rename(TESTDIR, ".");
	}
}

static
void
rename_dotdot(void)
{
	int rv;
	rv = rename("..", TESTDIR);
	report_test(rv, errno, EINVAL, "rename ..");
	if (rv==0) {
		/* oops... put it back */
		rename(TESTDIR, "..");
	}
}

static
void
rename_empty1(void)
{
	int rv;
	rv = rename("", TESTDIR);
	report_test2(rv, errno, EISDIR, EINVAL, "rename empty string");
	if (rv==0) {
		/* don't try to remove it */
		rename(TESTDIR, TESTDIR "-foo");
	}
}

static
void
rename_empty2(void)
{
	int rv;
	if (create_testdir()<0) {
		return;
	}
	rv = rename(TESTDIR, "");
	report_test2(rv, errno, EISDIR, EINVAL, "rename to empty string");
	rmdir(TESTDIR);
}

void
test_rename(void)
{
	test_rename_paths();

	rename_dot();
	rename_dotdot();
	rename_empty1();
	rename_empty2();
}

