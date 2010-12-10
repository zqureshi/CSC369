#include <stdlib.h>
#include <unistd.h>

/*
 * C standard function: panic exit from a user program.
 *
 * On most Unix systems, this sends the current process a fatal signal.
 * We can't do that (no signals in OS/161) so we just exit with a
 * nonzero exit code, skipping any libc cleanup.
 */

void
abort(void)
{
	_exit(255);
}
