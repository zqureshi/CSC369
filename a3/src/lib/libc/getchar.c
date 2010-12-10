#include <stdio.h>
#include <unistd.h>

/*
 * C standard I/O function - read character from stdin
 * and return it or the symbolic constant EOF (-1).
 */

int
getchar(void)
{
	char ch;
	int len;

	len = read(STDIN_FILENO, &ch, 1);
	if (len<=0) {
		/* end of file or error */
		return EOF;
	}

	/*
	 * Cast through unsigned char, to prevent sign extension. This
	 * sends back values on the range 0-255, rather than -128 to 127,
	 * so EOF can be distinguished from legal input.
	 */
	return (int)(unsigned char)ch;
}
