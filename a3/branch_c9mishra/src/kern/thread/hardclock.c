#include <types.h>
#include <lib.h>
#include <machine/spl.h>
#include <thread.h>
#include <clock.h>

/* 
 * The address of lbolt has thread_wakeup called on it once a second.
 */
int lbolt;

static int lbolt_counter;

/*
 * This is called HZ times a second by the timer device setup.
 */

void
hardclock(void)
{
	/*
	 * Collect statistics here as desired.
	 */


	lbolt_counter++;
	if (lbolt_counter >= HZ) {
		lbolt_counter = 0;
		thread_wakeup(&lbolt);
	}

	thread_yield();
}

/*
 * Suspend execution for n seconds.
 */
void
clocksleep(int num_secs)
{
	int s;

	s = splhigh();
	while (num_secs > 0) {
		thread_sleep(&lbolt);
		num_secs--;
	}
	splx(s);
}
