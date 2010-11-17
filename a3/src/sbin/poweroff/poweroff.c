#include <unistd.h>

/*
 * poweroff - shut down system and turn off power.
 * Usage: poweroff
 *
 * Just calls reboot() with the RB_POWEROFF flag.
 */

int
main()
{
	reboot(RB_POWEROFF);
	return 0;
}
