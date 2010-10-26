/*
 * Code for attaching the (generic) rtclock device to the LAMEbus ltimer.
 *
 * rtclock is a generic clock interface that gets its clock service from
 * an actual hardware clock of some kind. (Theoretically it could also
 * get its clock service from a clock maintained in software, as is the
 * case on most systems. However, no such driver has been written yet.)
 *
 * ltimer can provide this clock service.
 */

#include <types.h>
#include <lib.h>
#include <generic/rtclock.h>
#include <lamebus/ltimer.h>
#include "autoconf.h"

struct rtclock_softc *
attach_rtclock_to_ltimer(int rtclockno, struct ltimer_softc *ls)
{
	/*
	 * No need to probe; ltimer always has a clock.
	 * Just allocate the rtclock, set our fields, and return it.
	 */
	struct rtclock_softc *rtc = kmalloc(sizeof(struct rtclock_softc));
	if (rtc==NULL) {
		/* Out of memory */
		return NULL;
	}

	(void)rtclockno;  // unused

	rtc->rtc_devdata = ls;
	rtc->rtc_gettime = ltimer_gettime;

	return rtc;
}
