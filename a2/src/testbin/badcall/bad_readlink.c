/*
 * readlink
 */

#include <unistd.h>
#include <errno.h>

#include "test.h"

static
void
readlink_file(void)
{
	char buf[128];
	int fd, rv;

	fd = open_testfile("the question contains an invalid assumption");
	if (fd<0) {
		return;
	}
	close(fd);
	rv = readlink(TESTFILE, buf, sizeof(buf));
	report_test(rv, errno, EINVAL, "readlink on file");
	remove(TESTFILE);
}

static
void
readlink_dir(void)
{
	char buf[128];
	int rv;
	rv = readlink(".", buf, sizeof(buf));
	report_test(rv, errno, EISDIR, "readlink on .");
}

static
void
readlink_empty(void)
{
	char buf[128];
	int rv;
	rv = readlink("", buf, sizeof(buf));
	report_test2(rv, errno, EISDIR, EINVAL, "readlink on empty string");
}

void
test_readlink(void)
{
	test_readlink_path();
	test_readlink_buf();

	readlink_file();
	readlink_dir();
	readlink_empty();
}

