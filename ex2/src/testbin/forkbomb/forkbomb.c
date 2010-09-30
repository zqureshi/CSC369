/*
 * forkbomb - apply malthus to an operating system ;-)
 *
 * DO NOT RUN THIS ON A REAL SYSTEM - IT WILL GRIND TO A HALT AND
 * PEOPLE WILL COME AFTER YOU WIELDING BASEBALL BATS OR THE AD
 * BOARD(*). WE WARNED YOU.
 *
 * We don't expect your system to withstand this without grinding to
 * a halt, but once your basic system calls are complete it shouldn't
 * crash. Likewise for after your virtual memory system is complete.
 *
 * (...at least in an ideal world. However, it can be difficult to
 * handle all the loose ends involved. Heroic measures are not
 * expected. If in doubt, talk to the course staff.)
 *
 *
 * (*) The Administrative Board of Harvard College handles formal
 * disciplinary action.
 */

#include <unistd.h>
#include <err.h>

static volatile int pid;

int
main()
{
	int i;

	while (1) {
		fork();

		pid = getpid();

		/* Make sure each fork has its own address space. */
		for (i=0; i<300; i++) {
			volatile int seenpid;
			seenpid = pid;
			if (seenpid != getpid()) {
				errx(1, "pid mismatch (%d, should be %d) "
				     "- your vm is broken!", 
				     seenpid, getpid());
			}
		}
	}
}
