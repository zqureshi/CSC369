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
		err(1, "%s", file);
	}
}

int
main(int argc, char *argv[])
{
	int i;

	if (argc<2) {
		/* Must have at least one file. */
		errx(1, "Usage: rm FILES");
	}

	/* Just delete everything on the command line. */
	for (i=1; i<argc; i++) {
		doremove(argv[i]);
	}

	return 0;
}
