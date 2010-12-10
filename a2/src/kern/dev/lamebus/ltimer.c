/*
 * Driver for LAMEbus clock/timer card
 */
#include <types.h>
#include <lib.h>
#include <clock.h>
#include <machine/bus.h>
#include <lamebus/ltimer.h>
#include "autoconf.h"

/* Registers (offsets within slot) */
#define LT_REG_SEC    0     /* time of day: seconds */
#define LT_REG_NSEC   4     /* time of day: nanoseconds */
#define LT_REG_ROE    8     /* Restart On countdown-timer Expiry flag */
#define LT_REG_IRQ    12    /* Interrupt status register */
#define LT_REG_COUNT  16    /* Time for countdown timer (usec) */
#define LT_REG_SPKR   20    /* Beep control */

/* Granularity of countdown timer (usec) */
#define LT_GRANULARITY   1000000


static int haveclock=0;

/*
 * Setup routine called by autoconf stuff when an ltimer is found.
 */
int
config_ltimer(struct ltimer_softc *lt, int ltimerno)
{
	/*
	 * If we don't have a timer doing hardclock yet, use this one.
	 * (hardclock is the forced context switch code.)
	 */
	if (!haveclock) {
		haveclock = 1;
		lt->lt_hardclock = 1;

		/*
		 * Arm the timer to go off HZ times a second, and set
		 * it to autoreload (so we don't need to pay any more
		 * attention to it)
		 */

		bus_write_register(lt->lt_bus, lt->lt_buspos, LT_REG_ROE, 1);
		bus_write_register(lt->lt_bus, lt->lt_buspos, LT_REG_COUNT,
				   LT_GRANULARITY/HZ);

		kprintf("\nhardclock on ltimer%d (%u hz)", ltimerno, HZ);
	}
	else {
		/*
		 * Second and subsequent timers are not used for hardclock.
		 */
		lt->lt_hardclock = 0;
	}

	return 0;
}

/*
 * Interrupt handler.
 */
void
ltimer_irq(void *vlt)
{
	struct ltimer_softc *lt = vlt;
	u_int32_t val;

	val = bus_read_register(lt->lt_bus, lt->lt_buspos, LT_REG_IRQ);
	if (val) {
		/*
		 * Only call hardclock if we're responsible for hardclock.
		 * (Any additional timer devices are unused.)
		 */
		if (lt->lt_hardclock) {
			hardclock();
		}
	}
}

/*
 * The timer device will beep if you write to the beep register. It
 * doesn't matter what value you write. This function is called if
 * the beep device is attached to this timer. 
 */
void
ltimer_beep(void *vlt)
{
	struct ltimer_softc *lt = vlt;

	bus_write_register(lt->lt_bus, lt->lt_buspos, LT_REG_SPKR, 440);
}

/*
 * The timer device also has a realtime clock on it.
 * This function gets called if the rtclock device is attached
 * to this timer.
 */
void
ltimer_gettime(void *vlt, time_t *secs, u_int32_t *nsecs)
{
	struct ltimer_softc *lt = vlt;
	u_int32_t secs1, secs2;

	/*
	 * Read the seconds twice, on either side of the nanoseconds. 
	 * If nsecs is small, use the *later* value of seconds, in case
	 * the nanoseconds turned over between the time we got the earlier
	 * value and the time we got nsecs.
	 *
	 * Note that the clock in the ltimer device is accurate down
	 * to a single processor cycle, so this might actually matter
	 * now and then.
	 */

	secs1 = bus_read_register(lt->lt_bus, lt->lt_buspos,
				  LT_REG_SEC);
	*nsecs = bus_read_register(lt->lt_bus, lt->lt_buspos,
				   LT_REG_NSEC);
	secs2 = bus_read_register(lt->lt_bus, lt->lt_buspos,
				  LT_REG_SEC);

	if (*nsecs < 5000000) {
		*secs = secs2;
	}
	else {
		*secs = secs1;
	}
}
