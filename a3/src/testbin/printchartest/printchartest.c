#include <unistd.h>
#include <string.h>

/*
 * printchartest - Test implementation of the printchar syscall
 * Usage: printchartest
 *
 * Just calls printchar() to print out every character in a string.
 * Stops if the return value from printchar is not 1.
 */

int
main()
{
	int result;
	const char *teststring = "This is a test of printchar.END.\n";
	int len = strlen(teststring);
	int i;

	for (i=0; i < len; i++) {
	  result = printchar(teststring[i]);
	  if (result != 1) {
	    break;
	  }
	}

	return 0;
}
