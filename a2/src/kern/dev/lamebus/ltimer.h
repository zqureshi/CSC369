#ifndef _LAMEBUS_LTIMER_H_
#define _LAMEBUS_LTIMER_H_

/*
 * Hardware device data for LAMEbus timer device
 */
struct ltimer_softc {
	/* Initialized by config function */
	int lt_hardclock;        /* true if we should call hardclock() */

	/* Initialized by lower-level attach routine */
	void *lt_bus;		/* bus we're on */
	u_int32_t lt_buspos;	/* position (slot) on that bus */
	
};

/* Functions called by lower-level drivers */
void ltimer_irq(/*struct ltimer_softc*/ void *lt);  // interrupt handler

/* Functions called by higher-level devices */
void ltimer_beep(/*struct ltimer_softc*/ void *devdata);   // for beep device
void ltimer_gettime(/*struct ltimer_softc*/ void *devdata,
		    time_t *secs, u_int32_t *nsecs);       // for rtclock

#endif /* _LAMEBUS_LTIMER_H_ */
