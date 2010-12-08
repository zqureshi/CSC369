#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <generic/beep.h>
#include "autoconf.h"

/*
 * Machine-independent generic beep "device".
 *
 * Basically, all we do is remember something that can be used for
 * beeping, and provide the beep() function to the rest of the kernel.
 *
 * The kernel config mechanism can be used to explicitly choose which
 * of the available beeping devices to use, if more than one is
 * available.
 */

static struct beep_softc *the_beep = NULL;

int
config_beep(struct beep_softc *bs, int unit)
{
	/* We use only the first beep device. */
	if (unit!=0) {
		return ENODEV;
	}

	assert(the_beep==NULL);
	the_beep = bs;
	return 0;
}

void
beep(void)
{
	if (the_beep!=NULL) {
		the_beep->bs_beep(the_beep->bs_devdata);
	}
	else {
		kprintf("beep: Warning: no beep device\n");
	}
}
