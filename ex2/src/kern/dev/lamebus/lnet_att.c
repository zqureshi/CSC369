#include <types.h>
#include <lib.h>
#include <lamebus/lamebus.h>
#include "autoconf.h"

/* Lowest revision we support */
#define LOW_VERSION   1
/* Highest revision we support */
#define HIGH_VERSION  1

struct lnet_softc *
attach_lnet_to_lamebus(int lnetno, struct lamebus_softc *sc)
{
	int slot = lamebus_probe(sc, LB_VENDOR_CS161, LBCS161_NET,
				 LOW_VERSION, HIGH_VERSION);
	if (slot < 0) {
		return NULL;
	}

	kprintf("lnet%d: No network support in system\n", lnetno);

	return NULL;
}
