/*
 * Routine for probing/attaching ltimer to LAMEbus.
 */
#include <types.h>
#include <lib.h>
#include <lamebus/lamebus.h>
#include <lamebus/ltimer.h>
#include "autoconf.h"

/* Lowest revision we support */
#define LOW_VERSION   1
/* Highest revision we support */
#define HIGH_VERSION  1

struct ltimer_softc *
attach_ltimer_to_lamebus(int ltimerno, struct lamebus_softc *sc)
{
	struct ltimer_softc *lt;
	int slot = lamebus_probe(sc, LB_VENDOR_CS161, LBCS161_TIMER,
				 LOW_VERSION, HIGH_VERSION);
	if (slot < 0) {
		/* No ltimer (or no additional ltimer) found */
		return NULL;
	}

	lt = kmalloc(sizeof(struct ltimer_softc));
	if (lt==NULL) {
		/* out of memory */
		return NULL;
	}

	(void)ltimerno;  // unused

	/* Record what bus it's on */
	lt->lt_bus = sc;
	lt->lt_buspos = slot;

	/* Mark the slot in use and hook that slot's interrupt */
	lamebus_mark(sc, slot);
	lamebus_attach_interrupt(sc, slot, lt, ltimer_irq);

	return lt;
}
