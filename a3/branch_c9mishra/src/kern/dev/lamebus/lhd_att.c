/*
 * Code for probe/attach of lhd to LAMEbus.
 */
#include <types.h>
#include <lib.h>
#include <lamebus/lamebus.h>
#include <lamebus/lhd.h>
#include "autoconf.h"

/* Lowest revision we support */
#define LOW_VERSION   2
/* Highest revision we support */
#define HIGH_VERSION  2

struct lhd_softc *
attach_lhd_to_lamebus(int lhdno, struct lamebus_softc *sc)
{
	struct lhd_softc *lh;
	int slot = lamebus_probe(sc, LB_VENDOR_CS161, LBCS161_DISK,
				 LOW_VERSION, HIGH_VERSION);
	if (slot < 0) {
		/* None found */
		return NULL;
	}

	lh = kmalloc(sizeof(struct lhd_softc));
	if (lh==NULL) {
		/* Out of memory */
		return NULL;
	}

	/* Record what the lhd is attached to */
	lh->lh_busdata = sc;
	lh->lh_buspos = slot;
	lh->lh_unit = lhdno;

	/* Mark the slot in use and collect interrupts */
	lamebus_mark(sc, slot);
	lamebus_attach_interrupt(sc, slot, lh, lhd_irq);

	return lh;
}
