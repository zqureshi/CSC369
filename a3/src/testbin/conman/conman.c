/*
 * conman.c
 *
 * Echoes characters until a 'q' is read.
 * This should work once the basic system calls are implemented.
 */

#include <unistd.h>
#include <err.h>

int
main() {
	char ch=0;
	int len;

	while (ch!='q') {
		len = read(STDIN_FILENO, &ch, 1);
		if (len < 0) {
			err(1, "stdin: read");
		}
		if (len==0) {
			/* EOF */
			break;
		}
		write(STDOUT_FILENO, &ch, 1);
	}
	return 0;
}
