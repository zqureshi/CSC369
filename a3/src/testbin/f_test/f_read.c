/*
 * f_read.c
 *
 *	This used to be a separate binary, because it came from Nachos
 *	and nachos didn't support fork(). However, in OS/161 there's
 *	no reason to make it a separate binary; doing so just makes
 *	the test flaky.
 *
 *
 * 	it will start reading from a given file, concurrently
 * 	with other instances of f_read and f_write.
 *
 */

#define SectorSize   512

#define TMULT        50
#define FSIZE        ((SectorSize + 1) * TMULT)

#define FNAME        "f-testfile"
#define READCHAR     'r'
#define WRITECHAR    'w'

#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include "f_hdr.h"

static char buffer[SectorSize + 1];

static
void
check_buffer(void)
{
	int i;
	char ch = buffer[0];

	for (i = 1; i < SectorSize + 1; i++) {
		if (buffer[i] != ch) {
			errx(1, "Read error: %s", buffer);
		}
	}
	
	putchar(ch);
}

void
subproc_read(void)
{
	int fd;
	int i, res;
      
	printf("File Reader starting ...\n\n");

	fd = open(FNAME, O_RDONLY);
	if (fd < 0) {
		err(1, "%s: open", FNAME);
	}

	for (i=0; i<TMULT; i++) {
		res = read(fd, buffer, SectorSize + 1);
		if (res < 0) {
			err(1, "%s: read", FNAME);
		}

		// yield();

		if (res != SectorSize + 1) {
			errx(1, "%s: read: short count", FNAME);
		}
		check_buffer();
	}

	close(fd);

	printf("File Read exited successfully!\n");
}
