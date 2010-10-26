/*
 * triple.c
 *
 * 	Runs three copies of some subprogram.
 */

#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include "triple.h"

static
pid_t
spawnv(const char *prog, char **argv)
{
	pid_t pid = fork();
	switch (pid) {
	    case -1:
		err(1, "fork");
	    case 0:
		/* child */
		execv(prog, argv);
		err(1, "%s: execv", prog);
	    default:
		/* parent */
		break;
	}
	return pid;
}

static
int
dowait(int index, int pid)
{
	int status;

	if (waitpid(pid, &status, 0)<0) {
		warn("waitpid for copy #%d (pid %d)", index, pid);
		return 1;
	}
	else if (status != 0) {
		warnx("copy #%d (pid %d): exit %d", index, pid, status);
		return 1;
	}
	return 0;
}

void
triple(const char *prog)
{
	pid_t pids[3];
	int i, failures = 0;
	char *args[2];

	/* set up the argv */
	args[0]=(char *)prog;
	args[1]=NULL;

	warnx("Starting: running three copies of %s...", prog);

	for (i=0; i<3; i++) {
		pids[i]=spawnv(args[0], args);
	}

	for (i=0; i<3; i++) {
		failures += dowait(i, pids[i]);
	}

	if (failures > 0) {
		warnx("%d failures", failures);
	}
	else {
		warnx("Congratulations! You passed.");
	}
}

