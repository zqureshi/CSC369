#include <unistd.h>

/*
 * exittest - Test implementation of the _exit syscall
 * Usage: exittest
 *
 * Just calls _exit().
 *
 * Ok, so this one is kind of silly.
 */

int
main()
{

	_exit(42);

	/* If we get back here, exit returned, which it shouldn't do. */
	/* shutdown the system. */

	reboot(RB_POWEROFF);
}
