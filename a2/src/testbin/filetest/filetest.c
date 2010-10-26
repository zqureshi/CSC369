/*
 * filetest.c
 *
 * 	Tests the filesystem by opening, writing to and reading from a 
 * 	user specified file.
 *
 * This should run (on SFS) even before the file system assignment is started.
 * It should also continue to work once said assignment is complete.
 * It will not run fully on emufs, because emufs does not support remove().
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

int
main(int argc, char *argv[])
{
	static char writebuf[40] = "Twiddle dee dee, Twiddle dum dum.......\n";
	static char readbuf[41];

	int fd, rv;

	if (argc!=2) {
		errx(1, "Usage: filetest <filename>");
	}

	fd = open(argv[1], O_WRONLY|O_CREAT|O_TRUNC, 0664);
	if (fd<0) {
		err(1, "%s: open for write", argv[1]);
	}


	rv = write(fd, writebuf, 40);
	if (rv<0) {
		err(1, "%s: write", argv[1]);
	}

	rv = close(fd);
	if (rv<0) {
		err(1, "%s: close (1st time)", argv[1]);
	}

	fd = open(argv[1], O_RDONLY);
	if (fd<0) { 
		err(1, "%s: open for read", argv[1]);
	}

	rv = read(fd, readbuf, 40);
	if (rv<0) {
		err(1, "%s: read", argv[1]);
	}
	rv = close(fd);
	if (rv<0) {
		err(1, "%s: close (2nd time)", argv[1]);
	}
	/* ensure null termination */
	readbuf[40] = 0;

	if (strcmp(readbuf, writebuf)) {
		errx(1, "Buffer data mismatch!");
	}

	rv = remove(argv[1]);
	if (rv<0) {
		err(1, "%s: remove", argv[1]);
	}
	printf("Passed filetest.\n");
	return 0;
}
