/*
 * Code for probe/attach of lscreen to LAMEbus.
 */
#include <types.h>
#include <lib.h>
#include <lamebus/lamebus.h>
#include <lamebus/lscreen.h>
#include "autoconf.h"

/* Lowest revision we support */
#define LOW_VERSION   1
/* Highest revision we support */
#define HIGH_VERSION  1

struct lscreen_softc *
attach_lscreen_to_lamebus(int lscreenno, struct lamebus_softc *sc)
{
	struct lscreen_softc *ls;
	int slot = lamebus_probe(sc, LB_VENDOR_CS161, LBCS161_SCREEN,
				 LOW_VERSION, HIGH_VERSION);
	if (slot < 0) {
		/* Not found */
		return NULL;
	}

	ls = kmalloc(sizeof(struct lscreen_softc));
	if (ls==NULL) {
		/* Out of memory */
		return NULL;
	}

	/* Record what it's attached to */
	ls->ls_busdata = sc;
	ls->ls_buspos = slot;

	/* Mark the slot in use and hook the interrupt */
	lamebus_mark(sc, slot);
	lamebus_attach_interrupt(sc, slot, ls, lscreen_irq);

	return ls;
}
