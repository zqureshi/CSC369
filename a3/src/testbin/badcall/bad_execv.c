/*
 * bad calls to execv()
 */

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

#include "config.h"
#include "test.h"

static
int
exec_common_fork(void)
{
	int pid, rv, status;

	pid = fork();
	if (pid<0) {
		warn("UH-OH: fork failed");
		return -1;
	}
	
	if (pid==0) {
		/* child */
		return 0;
	}

	rv = waitpid(pid, &status, 0);
	if (rv == -1) {
		warn("UH-OH: waitpid failed");
		return -1;
	}
	if (status != MAGIC_STATUS) {
		warnx("FAILURE: wrong exit code of subprocess");
	}
	return 1;
}

static
void
exec_badprog(const void *prog, const char *desc)
{
	int rv;
	char *args[2];
	args[0] = (char *)"foo";
	args[1] = NULL;

	if (exec_common_fork() != 0) {
		return;
	}

	rv = execv(prog, args);
	report_test(rv, errno, EFAULT, desc);
	exit(MAGIC_STATUS);
}

static
void
exec_emptyprog(void)
{
	int rv;
	char *args[2];
	args[0] = (char *)"foo";
	args[1] = NULL;

	if (exec_common_fork() != 0) {
		return;
	}

	rv = execv("", args);
	report_test2(rv, errno, EINVAL, EISDIR, "exec the empty string");
	exit(MAGIC_STATUS);
}

static
void
exec_badargs(void *args, const char *desc)
{
	int rv;

	if (exec_common_fork() != 0) {
		return;
	}

	rv = execv("/bin/true", args);
	report_test(rv, errno, EFAULT, desc);
	exit(MAGIC_STATUS);
}

static
void
exec_onearg(void *ptr, const char *desc)
{
	int rv;

	char *args[3];
	args[0] = (char *)"foo";
	args[1] = (char *)ptr;
	args[2] = NULL;

	if (exec_common_fork() != 0) {
		return;
	}

	rv = execv("/bin/true", args);
	report_test(rv, errno, EFAULT, desc);
	exit(MAGIC_STATUS);
}

void
test_execv(void)
{
	exec_badprog(NULL, "exec NULL");
	exec_badprog(INVAL_PTR, "exec invalid pointer");
	exec_badprog(KERN_PTR, "exec kernel pointer");

	exec_emptyprog();

	exec_badargs(NULL, "exec /bin/true with NULL arglist");
	exec_badargs(INVAL_PTR, "exec /bin/true with invalid pointer arglist");
	exec_badargs(KERN_PTR, "exec /bin/true with kernel pointer arglist");

	exec_onearg(INVAL_PTR, "exec /bin/true with invalid pointer arg");
	exec_onearg(KERN_PTR, "exec /bin/true with kernel pointer arg");
}
