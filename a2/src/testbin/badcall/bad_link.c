/*
 * link
 */

#include <unistd.h>
#include <errno.h>

#include "test.h"

static
void
link_dir(void)
{
	int rv;
	rv = link(".", TESTDIR);
	report_test(rv, errno, EINVAL, "hard link of .");
	if (rv==0) {
		/* this might help recover... maybe */
		remove(TESTDIR);
	}
}

static
void
link_empty1(void)
{
	int rv;
	rv = link("", TESTDIR);
	report_test(rv, errno, EINVAL, "hard link of empty string");
}

static
void
link_empty2(void)
{
	int rv;
	if (create_testdir()<0) {
		return;
	}
	rv = link(TESTDIR, "");
	report_test(rv, errno, EINVAL, "hard link to empty string");
	rmdir(TESTDIR);
}

void
test_link(void)
{
	test_link_paths();
	link_dir();
	link_empty1();
	link_empty2();
}
