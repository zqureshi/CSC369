#include <unistd.h>
#include <err.h>

/*
 * rmdir - remove a directory
 * Usage: rmdir DIRECTORY
 *
 * Just calls the rmdir() system call.
 */

int
cmd_rmdir(int argc, char *argv[])
{
	if (argc!=2) {
		warnx("Usage: rmdir DIRECTORY");
	}

	if (rmdir(argv[1])) {
		warn("rmdir - %s", argv[1]);
	}
	return 0;
}
