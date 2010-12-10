#include <unistd.h>

/*
 * halt - shut down system, do not reboot, do not turn off power.
 * Usage: halt
 *
 * Just calls reboot() with the RB_HALT flag.
 */

int
main()
{
	reboot(RB_HALT);
	return 0;
}
