/*
 * Attachment code for having the generic beep device use the LAMEbus
 * ltimer device for beeping.
 */

#include <types.h>
#include <lib.h>
#include <generic/beep.h>
#include <lamebus/ltimer.h>
#include "autoconf.h"

struct beep_softc *
attach_beep_to_ltimer(int beepno, struct ltimer_softc *ls)
{
	struct beep_softc *bs = kmalloc(sizeof(struct beep_softc));
	if (bs==NULL) {
		return NULL;
	}

	(void)beepno;  // unused

	bs->bs_devdata = ls;
	bs->bs_beep = ltimer_beep;

	return bs;
}
