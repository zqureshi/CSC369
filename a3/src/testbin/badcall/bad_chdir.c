/*
 * Invalid calls to chdir()
 */

#include <unistd.h>
#include <errno.h>
#include <err.h>

#include "test.h"

static
void
chdir_empty(void)
{
	int rv;

	/*
	 * This is actually valid by some interpretations.
	 */

	rv = chdir("");
	report_test2(rv, errno, EINVAL, 0, "chdir to empty string");
}

void
test_chdir(void)
{
	test_chdir_path();
	chdir_empty();
}

