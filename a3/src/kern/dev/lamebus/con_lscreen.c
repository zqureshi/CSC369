/*
 * Attachment code for having the generic console device use the LAMEbus
 * screen device.
 */

#include <types.h>
#include <lib.h>
#include <generic/console.h>
#include <lamebus/lscreen.h>
#include "autoconf.h"

struct con_softc *
attach_con_to_lscreen(int consno, struct lscreen_softc *ls)
{
	struct con_softc *cs = kmalloc(sizeof(struct con_softc));
	if (cs==NULL) {
		return NULL;
	}

	cs->cs_devdata = ls;
	cs->cs_send = lscreen_write;
	cs->cs_sendpolled = lscreen_write;

	ls->ls_devdata = cs;
	ls->ls_start = con_start;
	ls->ls_input = con_input;

	return cs;
}

