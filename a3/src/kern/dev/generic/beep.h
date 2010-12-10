#ifndef _GENERIC_BEEP_H_
#define _GENERIC_BEEP_H_

/*
 * The device info for the generic MI beep device - a function
 * to call and a context pointer for it.
 */

struct beep_softc {
	void *bs_devdata;
	void (*bs_beep)(void *devdata);
};

#endif /* _GENERIC_BEEP_H_ */
