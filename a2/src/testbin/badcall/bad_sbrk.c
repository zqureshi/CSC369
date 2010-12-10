/*
 * sbrk
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
int
try_sbrk(long val)
{
	void *rv;
	rv = sbrk(val);
	if (rv==(void *)-1) {
		return errno;
	}
	return 0;
}

static
void
enforce_sbrk(long val, const char *desc, int err)
{
	int e;

	e = try_sbrk(val);
	if (e != err && e==0) {
		warnx("FAILURE: sbrk(%s): no error", desc);
		return;
	}
	if (e != err) {
		errno = e;
		warn("FAILURE: sbrk(%s): wrong error", desc);
		return;
	}
	warnx("passed: sbrk(%s)", desc);
}

static
void
sbrk_bigpos(void)
{
	enforce_sbrk(4096*1024*256, "huge positive", ENOMEM);
}

static
void
sbrk_bigneg(void)
{
	enforce_sbrk(-4096*1024*256, "huge negative", EINVAL);
}

static
void
sbrk_neg(void)
{
	enforce_sbrk(-8192, "too-large negative", EINVAL);
}

static
void
sbrk_unalignedpos(void)
{
	switch (try_sbrk(17)) {
	    case 0:
	    case EINVAL:
		warnx("passed: sbrk(unaligned positive)");
		break;
	    default:
		warn("FAILURE: sbrk(unaligned positive): wrong error");
		break;
	}
}

static
void
sbrk_unalignedneg(void)
{
	switch (try_sbrk(17)) {
	    case 0:
	    case EINVAL:
		warnx("passed: sbrk(unaligned negative)");
		break;
	    default:
		warn("FAILURE: sbrk(unaligned negative): wrong error");
		break;
	}
}

void
test_sbrk(void)
{
	sbrk_neg();
	sbrk_bigpos();
	sbrk_bigneg();
	sbrk_unalignedpos();
	sbrk_unalignedneg();
}

