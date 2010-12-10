/*
 * Machine-independent generic clock "device".
 *
 * Basically, all we do is remember something that can be used for
 * handling requests for the current time, and provide the gettime()
 * function to the rest of the kernel.
 *
 * The kernel config mechanism can be used to explicitly choose which
 * of the available clocks to use, if more than one is available.
 *
 * The system will panic if gettime() is called and there is no clock.
 */

#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <clock.h>
#include <generic/rtclock.h>
#include "autoconf.h"

static struct rtclock_softc *the_clock = NULL;

int
config_rtclock(struct rtclock_softc *rtc, int unit)
{
	/* We use only the first clock device. */
	if (unit!=0) {
		return ENODEV;
	}

	assert(the_clock==NULL);
	the_clock = rtc;
	return 0;
}

void
gettime(time_t *secs, u_int32_t *nsecs)
{
	assert(the_clock!=NULL);
	the_clock->rtc_gettime(the_clock->rtc_devdata, secs, nsecs);
}
