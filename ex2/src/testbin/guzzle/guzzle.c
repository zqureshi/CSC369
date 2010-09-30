/*
 * guzzle: Sit in the background in a long loop and display 
 * characters on the console occasionally.
 *
 * This is intended to help with scheduler testing.
 */

#include <stdio.h>
#include <unistd.h>

#define  TOP     1<<25  /* real long loop */

/* This program is a ridiculous, compute-bound program */
/* We should schedule this at low priorities. */

int
main(int argc, char *argv[])
{
	volatile int i;
	int ch = '+';

	if (argc == 2) {
		ch = argv[1][0];
	}

	for (i=0; i<TOP; i++) {	/* tight loop */
				/* that eats up cycles */
		if ( (i%10000) == 0) {
			putchar(ch);
		}
	}
	return 0;
}
