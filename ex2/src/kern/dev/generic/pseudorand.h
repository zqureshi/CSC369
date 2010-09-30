#ifndef _GENERIC_PSEUDORAND_H_
#define _GENERIC_PSEUDORAND_H_

struct uio;

struct pseudorand_softc {
	u_int32_t ps_seed;
};

u_int32_t pseudorand_random(/*struct pseudorand_softc*/ void *devdata);
u_int32_t pseudorand_randmax(/*struct pseudorand_softc*/ void *devdata);
int pseudorand_read(/*struct pseudorand_softc*/ void *, struct uio *);

#endif /* _GENERIC_PSEUDORAND_H_ */
