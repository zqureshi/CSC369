/*
 * Code for probe/attach of the emu device to lamebus.
 */

#include <types.h>
#include <lib.h>
#include <lamebus/lamebus.h>
#include <lamebus/emu.h>
#include "autoconf.h"

/* Lowest revision we support */
#define LOW_VERSION   1
/* Highest revision we support */
#define HIGH_VERSION  1

struct emu_softc *
attach_emu_to_lamebus(int emuno, struct lamebus_softc *sc)
{
	struct emu_softc *es;
	int slot = lamebus_probe(sc, LB_VENDOR_CS161, LBCS161_EMUFS,
				 LOW_VERSION, HIGH_VERSION);
	if (slot < 0) {
		return NULL;
	}

	es = kmalloc(sizeof(struct emu_softc));
	if (es==NULL) {
		return NULL;
	}

	es->e_busdata = sc;
	es->e_buspos = slot;
	es->e_unit = emuno;

	lamebus_mark(sc, slot);
	lamebus_attach_interrupt(sc, slot, es, emu_irq);

	return es;
}
