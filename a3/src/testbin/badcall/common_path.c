/*
 * Calls with invalid pathnames
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <err.h>

#include "config.h"
#include "test.h"

static
int
open_badpath(const char *path)
{
	return open(path, O_RDONLY);
}

static
int
remove_badpath(const char *path)
{
	return remove(path);
}

static
int
rename_badpath1(const char *path)
{
	return rename(path, TESTFILE);
}

static
int
rename_badpath2(const char *path)
{
	return rename(TESTFILE, path);
}

static
int
link_badpath1(const char *path)
{
	return link(path, TESTFILE);
}

static
int
link_badpath2(const char *path)
{
	return link(TESTFILE, path);
}

static
int
mkdir_badpath(const char *path)
{
	return mkdir(path, 0775);
}

static
int
rmdir_badpath(const char *path)
{
	return rmdir(path);
}

static
int
chdir_badpath(const char *path)
{
	return chdir(path);
}

static
int
symlink_badpath1(const char *path)
{
	return symlink(path, TESTFILE);
}

static
int
symlink_badpath2(const char *path)
{
	return symlink(TESTFILE, path);
}

static
int
readlink_badpath(const char *path)
{
	char buf[128];
	return readlink(path, buf, sizeof(buf));
}

static
int
lstat_badpath(const char *name)
{
	struct stat sb;
	return lstat(name, &sb);
}

static
int
stat_badpath(const char *name)
{
	struct stat sb;
	return stat(name, &sb);
}

////////////////////////////////////////////////////////////

static
void
common_badpath(int (*func)(const char *path), int mk, int rm, const char *path,
	       const char *call, const char *pathdesc)
{
	char mydesc[128];
	int rv;

	if (mk) {
		if (create_testfile()<0) {
			return;
		}
	}

	snprintf(mydesc, sizeof(mydesc), "%s with %s path", call, pathdesc);
	rv = func(path);
	report_test(rv, errno, EFAULT, mydesc);

	if (mk || rm) {
		remove(TESTFILE);
	}
}

static
void
any_badpath(int (*func)(const char *path), const char *call, int mk, int rm)
{
	common_badpath(func, mk, rm, NULL, call, "NULL");
	common_badpath(func, mk, rm, INVAL_PTR, call, "invalid-pointer");
	common_badpath(func, mk, rm, KERN_PTR, call, "kernel-pointer");
}

////////////////////////////////////////////////////////////

/* functions with one pathname */
#define T(call) \
  void                                  \
  test_##call##_path(void)              \
  {                                     \
   	any_badpath(call##_badpath, #call, 0, 0); \
  }

T(open);
T(remove);
T(mkdir);
T(rmdir);
T(chdir);
T(readlink);
T(stat);
T(lstat);

/* functions with two pathnames */
#define T2(call) \
  void                                  \
  test_##call##_paths(void)             \
  {                                     \
   	any_badpath(call##_badpath1, #call "(arg1)", 0, 1); \
   	any_badpath(call##_badpath2, #call "(arg2)", 1, 1); \
  }

T2(rename);
T2(link);
T2(symlink);
