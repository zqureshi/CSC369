/*
 * Attachment code for having the generic random device use the LAMEbus
 * random device.
 */

#include <types.h>
#include <lib.h>
#include <generic/random.h>
#include <lamebus/lrandom.h>
#include "autoconf.h"

struct random_softc *
attach_random_to_lrandom(int randomno, struct lrandom_softc *ls)
{
	struct random_softc *rs = kmalloc(sizeof(struct random_softc));
	if (rs==NULL) {
		return NULL;
	}

	(void)randomno;  // unused

	rs->rs_devdata = ls;
	rs->rs_random = lrandom_random;
	rs->rs_randmax = lrandom_randmax;
	rs->rs_read = lrandom_read;

	return rs;
}
