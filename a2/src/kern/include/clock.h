#ifndef _CLOCK_H_
#define _CLOCK_H_

/*
 * Time-related definitions.
 *
 * hardclock() is called from the timer interrupt HZ times a second.
 * gettime() may be used to fetch the current time of day.
 * getinterval() computes the time from time1 to time2.
 */

/* hardclocks per second */
#if OPT_SYNCHPROBS
/* Make synchronization more exciting :) */
#define HZ  10000
#else
/* More realistic value */
#define HZ  100
#endif

void hardclock(void);

void gettime(time_t *seconds, u_int32_t *nanoseconds);

void getinterval(time_t secs1, u_int32_t nsecs,
		 time_t secs2, u_int32_t nsecs2,
		 time_t *rsecs, u_int32_t *rnsecs);

#endif /* _CLOCK_H_ */
