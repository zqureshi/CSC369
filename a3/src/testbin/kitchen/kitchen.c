/*
 * kitchen.c
 *
 * 	Run a bunch of sinks (only).
 *
 * This tests concurrent read access to the console driver.
 */

#include <unistd.h>
#include <err.h>

static char *sargv[2] = { (char *)"sink", NULL };

#define MAXPROCS  6
static int pids[MAXPROCS], npids;

static
void
sink(void)
{
	int pid = fork();
	switch (pid) {
	    case -1:
		err(1, "fork");
	    case 0:
		/* child */
		execv("/testbin/sink", sargv);
		err(1, "/testbin/sink");
	    default:
		/* parent */
		pids[npids++] = pid;
		break;
	}
}

static
void
waitall(void)
{
	int i, status;
	for (i=0; i<npids; i++) {
		if (waitpid(pids[i], &status, 0)<0) {
			warn("waitpid for %d", pids[i]);
		}
		else if (status != 0) {
			warnx("pid %d: exit %d", pids[i], status);
		}
	}
}


int
main(void)
{
	sink();
	sink();
	sink();
	sink();

	waitall();

	return 0;
}
