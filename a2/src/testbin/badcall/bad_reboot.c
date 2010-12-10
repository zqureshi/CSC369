/*
 * Invalid calls to reboot()
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
reboot_badflags(void)
{
	int rv;

	warnx("NOTICE: if this kills the system, it's a failure.");
	rv = reboot(15353);
	report_test(rv, errno, EINVAL, "reboot with invalid flags");
}

void
test_reboot(void)
{
	reboot_badflags();
}
