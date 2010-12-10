/*
 * Calls with invalid fds
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
read_badfd(int fd)
{
	char buf[128];
	return read(fd, buf, sizeof(buf));
}

static
int
write_badfd(int fd)
{
	char buf[128];
	memset(buf, 'a', sizeof(buf));
	return write(fd, buf, sizeof(buf));
}


static
int
close_badfd(int fd)
{
	return close(fd);
}

static
int
ioctl_badfd(int fd)
{
	return ioctl(fd, 0, NULL);
}

static
int
lseek_badfd(int fd)
{
	return lseek(fd, 0, SEEK_SET);
}

static
int
fsync_badfd(int fd)
{
	return fsync(fd);
}

static
int
ftruncate_badfd(int fd)
{
	return ftruncate(fd, 60);
}

static
int
fstat_badfd(int fd)
{
	struct stat sb;
	return fstat(fd, &sb);
}

static
int
getdirentry_badfd(int fd)
{
	char buf[32];
	return getdirentry(fd, buf, sizeof(buf));
}

static
int
dup2_badfd(int fd)
{
	/* use the +1 to avoid doing dup2(CLOSED_FD, CLOSED_FD) */
	return dup2(fd, CLOSED_FD+1);
}

static
void
dup2_cleanup(void)
{
	close(CLOSED_FD+1);
}

////////////////////////////////////////////////////////////

static
void
any_badfd(int (*func)(int fd), void (*cleanup)(void), const char *callname,
	  int fd, const char *fddesc)
{
	char fulldesc[128];
	int rv;

	snprintf(fulldesc, sizeof(fulldesc), "%s using %s", callname, fddesc);
	rv = func(fd);
	report_test(rv, errno, EBADF, fulldesc);
	if (cleanup) {
		cleanup();
	}
}

static
void
runtest(int (*func)(int fd), void (*cleanup)(void), const char *callname)
{
	/*
	 * If adding cases, also see bad_dup2.c
	 */

	/* basic invalid case: fd -1 */
	any_badfd(func, cleanup, callname, -1, "fd -1");

	/* also try -5 in case -1 is special somehow */
	any_badfd(func, cleanup, callname, -5, "fd -5");

	/* try a fd we know is closed */
	any_badfd(func, cleanup, callname, CLOSED_FD, "closed fd");

	/* try a positive fd we know is out of range */
	any_badfd(func, cleanup, callname, IMPOSSIBLE_FD, "impossible fd");

	/* test for off-by-one errors */
#ifdef OPEN_MAX
	any_badfd(func, cleanup, callname, OPEN_MAX, "fd OPEN_MAX");
#else
	warnx("Warning: OPEN_MAX not defined, test skipped");
#endif
}

////////////////////////////////////////////////////////////

#define T(call) \
  void                                          \
  test_##call##_fd(void)                        \
  {                                             \
   	runtest(call##_badfd, NULL, #call);     \
  }

#define TC(call) \
  void                                          \
  test_##call##_fd(void)                        \
  {                                             \
   	runtest(call##_badfd, call##_cleanup, #call);\
  }

T(read);
T(write);
T(close);
T(ioctl);
T(lseek);
T(fsync);
T(ftruncate);
T(fstat);
T(getdirentry);
TC(dup2);
