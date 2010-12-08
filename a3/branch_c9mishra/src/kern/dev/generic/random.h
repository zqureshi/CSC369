#ifndef _GENERIC_RANDOM_H_
#define _GENERIC_RANDOM_H_

#include <dev.h>
struct uio;

struct random_softc {
	/* Initialized by lower-level attach routine */
	void *rs_devdata;
	u_int32_t (*rs_random)(void *devdata);
	u_int32_t (*rs_randmax)(void *devdata);
	int (*rs_read)(void *devdata, struct uio *uio);

	struct device rs_dev;
};

#endif /* _GENERIC_RANDOM_H_ */
