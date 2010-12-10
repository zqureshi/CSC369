/*
 * farm.c
 *
 * 	Run a bunch of cpu pigs and one cat.
 *      The file it cats is "catfile". This should be created in advance.
 *
 * This test should itself run correctly when the basic system calls
 * are complete. It may be helpful for scheduler performance analysis.
 */

#include <unistd.h>
#include <err.h>

static char *hargv[2] = { (char *)"hog", NULL };
static char *cargv[3] = { (char *)"cat", (char *)"catfile", NULL };

#define MAXPROCS  6
static int pids[MAXPROCS], npids;

static
void
spawnv(const char *prog, char **argv)
{
	int pid = fork();
	switch (pid) {
	    case -1:
		err(1, "fork");
	    case 0:
		/* child */
		execv(prog, argv);
		err(1, "%s", prog);
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

static
void
hog(void)
{
	spawnv("/testbin/hog", hargv);
}

static
void
cat(void)
{
	spawnv("/bin/cat", cargv);
}

int
main()
{
	hog();
	hog();
	hog();
	cat();

	waitall();

	return 0;
}
