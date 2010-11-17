#ifndef _LAMEBUS_LRANDOM_H_
#define _LAMEBUS_LRANDOM_H_

struct uio;

struct lrandom_softc {
	/* Initialized by lower-level attach routine */
	void *lr_bus;
	u_int32_t lr_buspos;
};

/* Functions called by higher-level drivers */
u_int32_t lrandom_random(/*struct lrandom_softc*/ void *devdata);
u_int32_t lrandom_randmax(/*struct lrandom_softc*/ void *devdata);
int lrandom_read(/*struct lrandom_softc*/ void *, struct uio *);

#endif /* _LAMEBUS_LRANDOM_H_ */
