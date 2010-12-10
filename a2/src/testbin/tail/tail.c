/*
 * tail.c
 *
 * 	Outputs a file beginning at a specific location.
 *	Usage: tail <file> <location>
 *
 * This may be useful for testing during the file system assignment.
 */

#include <unistd.h>
#include <stdlib.h>
#include <err.h>

#define BUFSIZE 1000

/* Put buffer in data space.  We know that the program should allocate as */
/* much data space as required, but stack space is tight. */

char buffer[BUFSIZE];

static
void
tail(int file, off_t where, const char *filename)
{
	int len;

	if (lseek(file, where, SEEK_SET)<0) {
		err(1, "%s", filename);
	}
	
	while ((len = read(file, buffer, sizeof(buffer))) > 0) {
		write(STDOUT_FILENO, buffer, len);
	}
}

int
main(int argc, char **argv)
{
	int file;

	if (argc < 3) {
		errx(1, "Usage: tail <file> <location>");
	}
	file = open(argv[1], O_RDONLY);
	if (file < 0) {
		err(1, "%s", argv[1]);
	}
	tail(file, atoi(argv[2]), argv[1]);
	close(file);
	return 0;
}

