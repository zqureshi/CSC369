#include <unistd.h>
#include <err.h>

/*
 * rmdir - remove a directory
 * Usage: rmdir DIRECTORY
 *
 * Just calls the rmdir() system call.
 */

int
main(int argc, char *argv[])
{
	if (argc!=2) {
		errx(1, "Usage: rmdir DIRECTORY");
	}

	if (rmdir(argv[1])) {
		err(1, "%s", argv[1]);
	}
	return 0;
}
