#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <err.h>

/*
 * mkdir - create a directory.
 * Usage: mkdir DIRECTORY
 *
 * Just calls the mkdir() system call.
 */

int
main(int argc, char *argv[])
{
	if (argc!=2) {
		errx(1, "Usage: mkdir DIRECTORY");
	}

	if (mkdir(argv[1], 0775)) {
		err(1, "%s", argv[1]);
	}
	return 0;
}
