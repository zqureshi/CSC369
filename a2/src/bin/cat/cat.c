#include <unistd.h>
#include <string.h>
#include <err.h>

/*
 * cat - concatenate and print
 * Usage: cat [files]
 */



/* Print a file that's already been opened. */
static
void
docat(const char *name, int fd)
{
	char buf[1024];
	int len, wr, wrtot;

	/*
	 * As long as we get more than zero bytes, we haven't hit EOF.
	 * Zero means EOF. Less than zero means an error occurred.
	 * We may read less than we asked for, though, in various cases
	 * for various reasons.
	 */
	while ((len = read(fd, buf, sizeof(buf)))>0) {
		/*
		 * Likewise, we may actually write less than we attempted
		 * to. So loop until we're done.
		 */
		wrtot = 0;
		while (wrtot < len) {
			wr = write(STDOUT_FILENO, buf+wrtot, len-wrtot);
			if (wr<0) {
				err(1, "stdout");
			}
			wrtot += wr;
		}
	}
	/*
	 * If we got a read error, print it and exit.
	 */
	if (len<0) {
		err(1, "%s", name);
	}
}

/* Print a file by name. */
static
void
cat(const char *file)
{
	int fd;

	/*
	 * "-" means print stdin.
	 */
	if (!strcmp(file, "-")) {
		docat("stdin", STDIN_FILENO);
		return;
	}

	/*
	 * Open the file, print it, and close it.
	 * Bail out if we can't open it.
	 */
	fd = open(file, O_RDONLY);
	if (fd<0) {
		err(1, "%s", file);
	}
	docat(file, fd);
	close(fd);
}


int
main(int argc, char *argv[])
{
	if (argc==1) {
		/* No args - just do stdin */
		docat("stdin", STDIN_FILENO);
	}
	else {
		/* Print all the files specified on the command line. */
		int i;
		for (i=1; i<argc; i++) {
			cat(argv[i]);
		}
	}
	return 0;
}
