#include <unistd.h>
#include <stdlib.h>

/*
 * false - fail.
 *
 * "All software sucks. Ok, so maybe /bin/true doesn't. But /bin/false
 * sure does - it fails all the time."
 */

int
main()
{
	/* Just exit with a failure code. */
	exit(1);
}
