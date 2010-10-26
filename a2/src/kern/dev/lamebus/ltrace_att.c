#include <types.h>
#include <lib.h>
#include <lamebus/lamebus.h>
#include <lamebus/ltrace.h>
#include "autoconf.h"

/* Lowest revision we support */
#define LOW_VERSION   1
/* Highest revision we support */
#define HIGH_VERSION  1

struct ltrace_softc *
attach_ltrace_to_lamebus(int ltraceno, struct lamebus_softc *sc)
{
	struct ltrace_softc *lt;
	int slot = lamebus_probe(sc, LB_VENDOR_CS161, LBCS161_TRACE,
				 LOW_VERSION, HIGH_VERSION);
	if (slot < 0) {
		return NULL;
	}

	lt = kmalloc(sizeof(struct ltrace_softc));
	if (lt==NULL) {
		return NULL;
	}

	(void)ltraceno;  // unused

	lt->lt_busdata = sc;
	lt->lt_buspos = slot;

	lamebus_mark(sc, slot);

	return lt;
}
