/*
 * f_write.c
 *
 *	This used to be a separate binary, because it came from Nachos
 *	and nachos didn't support fork(). However, in OS/161 there's
 *	no reason to make it a separate binary; doing so just makes
 *	the test flaky.
 *
 *
 * 	It will start writing into a file, concurrently with
 * 	one or more instances of f_read.
 *
 */

#define SectorSize   512

#define TMULT        50
#define FSIZE        ((SectorSize + 1) * TMULT)

#define FNAME        "f-testfile"
#define READCHAR     'r'
#define WRITECHAR    'w'

#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include "f_hdr.h"

static char buffer[SectorSize + 1];

void
subproc_write(void)
{
	int fd;
	int i;

	for (i=0; i < SectorSize + 1; i++) {
		buffer[i] = WRITECHAR;
	}
  
	printf("File Writer starting ...\n");

	fd = open(FNAME, O_WRONLY);
	if (fd < 0) {
		err(1, "%s: open", FNAME);
	}

	for (i=0; i<TMULT; i++) {
		// yield();
		write(fd, buffer, SectorSize + 1);
	}

	close(fd);

	printf("File Write exited successfully!\n");
}
