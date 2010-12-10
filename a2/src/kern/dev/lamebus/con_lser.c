/*
 * Attachment code for having the generic console device use the LAMEbus
 * serial device.
 */

#include <types.h>
#include <lib.h>
#include <generic/console.h>
#include <lamebus/lser.h>
#include "autoconf.h"

struct con_softc *
attach_con_to_lser(int consno, struct lser_softc *ls)
{
	struct con_softc *cs = kmalloc(sizeof(struct con_softc));
	if (cs==NULL) {
		return NULL;
	}

	(void)consno;  // unused

	cs->cs_devdata = ls;
	cs->cs_send = lser_write;
	cs->cs_sendpolled = lser_writepolled;

	ls->ls_devdata = cs;
	ls->ls_start = con_start;
	ls->ls_input = con_input;

	return cs;
}

