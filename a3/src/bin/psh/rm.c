#include <unistd.h>
#include <err.h>

/*
 * rm - remove (delete) files
 * Usage: rm file...
 */

/* Delete a single file. */
static
void
doremove(const char *file)
{
	if (remove(file)) {
		warn("rm - %s", file);
	}
}

int
cmd_rm(int argc, char *argv[])
{
	int i;

	if (argc<2) {
		/* Must have at least one file. */
		warnx("Usage: rm FILES");
		return 0;
	}

	/* Just delete everything on the command line. */
	for (i=1; i<argc; i++) {
		doremove(argv[i]);
	}

	return 0;
}
