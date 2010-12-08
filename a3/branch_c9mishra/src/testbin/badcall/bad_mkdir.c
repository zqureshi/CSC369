/*
 * mkdir
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
mkdir_dot(void)
{
	int rv;

	rv = mkdir(".", 0775);
	report_test(rv, errno, EEXIST, "mkdir .");
}

static
void
mkdir_dotdot(void)
{
	int rv;

	rv = mkdir("..", 0775);
	report_test(rv, errno, EEXIST, "mkdir ..");
}

static
void
mkdir_empty(void)
{
	int rv;
	rv = mkdir("", 0775);
	report_test(rv, errno, EINVAL, "mkdir of empty string");
}

void
test_mkdir(void)
{
	test_mkdir_path();

	mkdir_dot();
	mkdir_dotdot();
	mkdir_empty();
}
