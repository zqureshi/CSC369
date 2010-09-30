/*
 * sink.c
 *
 * 	Accept console input (or rather, stdin) and throw it away.
 *
 * This should work once the basic system calls are complete.
 */

#include <unistd.h>
#include <err.h>

int
main(void)
{
	char ch=0;
	int r;

	while (ch!='q') {
		r = read(STDIN_FILENO, &ch, 1);
		if (r<0) {
			err(1, "stdin");
		}
		if (r==0) {
			/* EOF */
			break;
		}
	}

	return 0;
}
