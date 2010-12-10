#ifndef _GENERIC_RTCLOCK_H_
#define _GENERIC_RTCLOCK_H_

/*
 * The device info for the generic MI clock device - a function
 * to call and a context pointer for it.
 */

struct rtclock_softc {
	void *rtc_devdata;
	void (*rtc_gettime)(void *devdata, time_t *secs, u_int32_t *nsecs);
};

#endif /* _GENERIC_RTCLOCK_H_ */
