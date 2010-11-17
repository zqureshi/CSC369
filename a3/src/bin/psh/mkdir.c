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
cmd_mkdir(int argc, char *argv[])
{
	if (argc!=2) {
		warnx("Usage: mkdir DIRECTORY");
		return 0;
	}

	if (mkdir(argv[1], 0775)) {
		warn("mkdir - %s", argv[1]);
	}
	return 0;
}
