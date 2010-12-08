/*
 * Bad calls to fstat, lstat, and stat
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

////////////////////////////////////////////////////////////

static
int
badbuf_fstat(struct stat *sb)
{
	return fstat(STDIN_FILENO, sb);
}

static
int
badbuf_lstat(struct stat *sb)
{
	return lstat("null:", sb);
}

static
int
badbuf_stat(struct stat *sb)
{
	return stat("null:", sb);
}

static
void
common_badbuf(int (*statfunc)(struct stat *), void *ptr,
	      const char *call, const char *ptrdesc)
{
	int rv;
	char mydesc[128];

	snprintf(mydesc, sizeof(mydesc), "%s with %s buf", call, ptrdesc);
	rv = statfunc(ptr);
	report_test(rv, errno, EFAULT, mydesc);
}

static
void
any_badbuf(int (*statfunc)(struct stat *), const char *call)
{
	common_badbuf(statfunc, NULL, call, "NULL");
	common_badbuf(statfunc, INVAL_PTR, call, "invalid pointer");
	common_badbuf(statfunc, KERN_PTR, call, "kernel pointer");
}

////////////////////////////////////////////////////////////

static
void
any_empty(int (*statfunc)(const char *, struct stat *), const char *call)
{
	struct stat sb;
	char desc[128];
	int rv;

	snprintf(desc, sizeof(desc), "%s on empty string", call);
	rv = statfunc("", &sb);
	report_test2(rv, errno, 0, EINVAL, desc);
}

////////////////////////////////////////////////////////////

void
test_fstat(void)
{
	test_fstat_fd();
	any_badbuf(badbuf_fstat, "fstat");
}

void
test_lstat(void)
{
	test_lstat_path();
	any_empty(lstat, "stat");
	any_badbuf(badbuf_lstat, "lstat");
}

void
test_stat(void)
{
	test_stat_path();
	any_empty(stat, "stat");
	any_badbuf(badbuf_stat, "stat");
}

