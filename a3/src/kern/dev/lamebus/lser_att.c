#include <types.h>
#include <lib.h>
#include <lamebus/lamebus.h>
#include <lamebus/lser.h>
#include "autoconf.h"

/* Lowest revision we support */
#define LOW_VERSION   1
/* Highest revision we support */
#define HIGH_VERSION  1

struct lser_softc *
attach_lser_to_lamebus(int lserno, struct lamebus_softc *sc)
{
	struct lser_softc *ls;
	int slot = lamebus_probe(sc, LB_VENDOR_CS161, LBCS161_SERIAL,
				 LOW_VERSION, HIGH_VERSION);
	if (slot < 0) {
		return NULL;
	}

	ls = kmalloc(sizeof(struct lser_softc));
	if (ls==NULL) {
		return NULL;
	}

	(void)lserno;  // unused

	ls->ls_busdata = sc;
	ls->ls_buspos = slot;

	lamebus_mark(sc, slot);
	lamebus_attach_interrupt(sc, slot, ls, lser_irq);

	return ls;
}
