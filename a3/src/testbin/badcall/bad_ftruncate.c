/*
 * ftruncate
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
ftruncate_fd_device(void)
{
	int rv, fd;

	fd = open("null:", O_RDWR);
	if (fd<0) {
		warn("UH-OH: opening null: failed");
		return;
	}

	rv = ftruncate(fd, 6);
	report_test(rv, errno, EINVAL, "ftruncate on device");

	close(fd);
}

static
void
ftruncate_size_neg(void)
{
	int rv, fd;

	fd = open_testfile(0);
	if (fd<0) {
		return;
	}

	rv = ftruncate(fd, -60);
	report_test(rv, errno, EINVAL, "ftruncate to negative size");

	close(fd);
	remove(TESTFILE);
}

void
test_ftruncate(void)
{
	test_ftruncate_fd();

	ftruncate_fd_device();
	ftruncate_size_neg();
}
