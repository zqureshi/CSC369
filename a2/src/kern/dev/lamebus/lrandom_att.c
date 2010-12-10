#include <types.h>
#include <lib.h>
#include <lamebus/lamebus.h>
#include <lamebus/lrandom.h>
#include "autoconf.h"

/* Lowest revision we support */
#define LOW_VERSION   1
/* Highest revision we support */
#define HIGH_VERSION  1

struct lrandom_softc *
attach_lrandom_to_lamebus(int lrandomno, struct lamebus_softc *sc)
{
	struct lrandom_softc *lr;
	int slot = lamebus_probe(sc, LB_VENDOR_CS161, LBCS161_RANDOM,
				 LOW_VERSION, HIGH_VERSION);
	if (slot < 0) {
		return NULL;
	}

	lr = kmalloc(sizeof(struct lrandom_softc));
	if (lr==NULL) {
		return NULL;
	}

	(void)lrandomno;  // unused

	lr->lr_bus = sc;
	lr->lr_buspos = slot;

	lamebus_mark(sc, slot);

	return lr;
}
