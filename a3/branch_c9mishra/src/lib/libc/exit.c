#include <stdlib.h>
#include <unistd.h>

/*
 * C standard function: exit process.
 */

void
exit(int code)
{
	/*
	 * In a more complicated libc, this would call functions registered
	 * with atexit() before calling the syscall to actually exit.
	 */

	_exit(code);
}

/*
 * The mips gcc we were using in 2001, and probably other versions as
 * well, knows more than is healthy: it knows without being told that
 * exit and _exit don't return.
 * 
 * This causes it to make foolish optimizations that cause broken
 * things to happen if _exit *does* return, as it does in the base
 * system (because it's unimplemented) and may also do if someone has
 * a bug.
 *
 * The way it works is that if _exit returns, execution falls into
 * whatever happens to come after exit(), with the registers set up in
 * such a way that when *that* function returns it actually ends up
 * calling itself again. This causes weird things to happen.
 *
 * This function has no purpose except to trap that
 * circumstance. Looping doing nothing is not entirely optimal, but
 * there's not much we *can* do, and it's better than jumping around
 * wildly as would happen if this function were removed.
 *
 * If you change this to loop calling _exit(), gcc "helpfully"
 * optimizes the loop away and the behavior reverts to that previously
 * described.
 */

void
__exit_hack(void)
{
	volatile int blah = 1;
	while (blah) {}
}
