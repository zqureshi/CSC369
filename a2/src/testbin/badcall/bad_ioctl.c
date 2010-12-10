/*
 * ioctl 
 */

#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "config.h"
#include "test.h"

static
void
one_ioctl_badbuf(int fd, int code, const char *codename,
		 void *ptr, const char *ptrdesc)
{
	char desc[128];
	int rv;

	snprintf(desc, sizeof(desc), "ioctl %s with %s", codename, ptrdesc);
	rv = ioctl(fd, code, ptr);
	report_test(rv, errno, EFAULT, desc);
}

static
void
any_ioctl_badbuf(int fd, int code, const char *codename)
{
	one_ioctl_badbuf(fd, code, codename, NULL, "NULL pointer");
	one_ioctl_badbuf(fd, code, codename, INVAL_PTR, "invalid pointer");
	one_ioctl_badbuf(fd, code, codename, KERN_PTR, "kernel pointer");
}

#define IOCTL(fd, sym) any_ioctl_badbuf(fd, sym, #sym)

static
void
ioctl_badbuf(void)
{
	/*
	 * Since we don't actually define any ioctls, this code won't
	 * actually run. But if you do define ioctls, turn these tests
	 * on for those that actually use the data buffer argument for
	 * anything.
	 */

	/* IOCTL(STDIN_FILENO, TIOCGETA); */


	/* suppress gcc warning */
	(void)any_ioctl_badbuf;
}

static
void
ioctl_badcode(void)
{
	int rv;
	rv = ioctl(STDIN_FILENO, NONEXIST_IOCTL, NULL);
	report_test(rv, errno, EIOCTL, "invalid ioctl");
}

void
test_ioctl(void)
{
	test_ioctl_fd();

	/* Since we don't actually define any ioctls, this is not meaningful */
	ioctl_badcode();
	ioctl_badbuf();
}
