/*
 * Driver for LAMEbus random generator card
 */
#include <types.h>
#include <lib.h>
#include <uio.h>
#include <machine/bus.h>
#include <lamebus/lrandom.h>
#include "autoconf.h"

/* Registers (offsets within slot) */
#define LR_REG_RAND   0     /* random register */

/* Constants */
#define LR_RANDMAX  0xffffffff

int
config_lrandom(struct lrandom_softc *lr, int lrandomno)
{
	(void)lrandomno;
	(void)lr;
	return 0;
}

u_int32_t
lrandom_random(void *devdata)
{
	struct lrandom_softc *lr = devdata;
	return bus_read_register(lr->lr_bus, lr->lr_buspos, LR_REG_RAND);
}

u_int32_t
lrandom_randmax(void *devdata)
{
	(void)devdata;
	return LR_RANDMAX;
}

int
lrandom_read(void *devdata, struct uio *uio)
{
	struct lrandom_softc *lr = devdata;
	int result;
	union {
		u_int32_t val;
		char buf[sizeof(u_int32_t)];
	} u;

	while (uio->uio_resid > 0) {
		u.val = bus_read_register(lr->lr_bus, lr->lr_buspos, 
					  LR_REG_RAND);
		result = uiomove(u.buf, sizeof(u.buf), uio);
		if (result) {
			return result;
		}
	}
	
	return 0;
}
