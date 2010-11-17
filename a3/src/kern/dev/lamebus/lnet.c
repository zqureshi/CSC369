#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include "autoconf.h"

int
config_lnet(struct lnet_softc *sc, int lnetno)
{
	(void)sc;
	
	kprintf("lnet%d: No network support in system\n", lnetno);

	return ENODEV;
}


