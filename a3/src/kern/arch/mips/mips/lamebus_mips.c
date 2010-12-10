#include <types.h>
#include <kern/unistd.h>
#include <lib.h>
#include <synch.h>
#include <machine/spl.h>
#include <machine/pcb.h>
#include <dev.h>
#include <machine/bus.h>
#include <lamebus/lamebus.h>
#include "autoconf.h"

/* LAMEbus data for the system (we have only one LAMEbus per system) */
static struct lamebus_softc *lamebus;

void
machdep_dev_bootstrap(void)
{
	/* Interrupts should be off (and have been off since startup) */
	assert(curspl>0);

	/* Initialize the system LAMEbus data */
	lamebus = lamebus_init();

	/*
	 * Print the device name for the main bus.
	 */
	kprintf("lamebus0 (system main bus)\n");

	/*
	 * Now we can take interrupts without croaking, so turn them on.
	 * Some device probes might require being able to get interrupts.
	 */

	spl0();

	/*
	 * Now probe all the devices attached to the bus.
	 * (This amounts to all devices.)
	 */
	autoconf_lamebus(lamebus, 0);
}

/*
 * Function to generate the memory address (in the uncached segment)
 * for the specified offset into the specified slot's region of the
 * LAMEbus.
 */
void *
lamebus_map_area(struct lamebus_softc *bus, int slot, u_int32_t offset)
{
	u_int32_t address;

	(void)bus;   // not needed

	assert(slot>=0 && slot<LB_NSLOTS);

	address = LB_BASEADDR + slot*LB_SLOT_SIZE + offset;
	return (void *)address;
}

/*
 * Read a 32-bit register from a LAMEbus device.
 */
u_int32_t
lamebus_read_register(struct lamebus_softc *bus, int slot, u_int32_t offset)
{
	u_int32_t *ptr = lamebus_map_area(bus, slot, offset);

	return *ptr;
}

/*
 * Write a 32-bit register of a LAMEbus device.
 */
void
lamebus_write_register(struct lamebus_softc *bus, int slot,
		       u_int32_t offset, u_int32_t val)
{
	u_int32_t *ptr = lamebus_map_area(bus, slot, offset);

	*ptr = val;
}


/*
 * Power off the system.
 */
void
md_poweroff(void)
{
	/*
	 *
	 * Note that lamebus_write_register() doesn't actually access
	 * the bus argument, so this will still work if we get here
	 * before the bus is initialized.
	 */
	lamebus_poweroff(lamebus);
}

/*
 * Reboot the system.
 */
void
md_reboot(void)
{
	/*
	 * The MIPS doesn't appear to have any on-chip reset.
	 * LAMEbus doesn't have a reset control, so we just
	 * power off instead of rebooting. This would not be
	 * so great in a real system, but it's fine for what
	 * we're doing.
	 */
	kprintf("Cannot reboot - powering off instead, sorry.\n");
	md_poweroff();
}

/*
 * Halt the system.
 * On some systems, this would return to the boot monitor. But we don't
 * have one.
 */
void
md_halt(void)
{
	cpu_halt();
}

/*
 * Called to reset the system from panic().
 *
 * By the time we get here, the system may well be sufficiently hosed
 * as to panic recursively if we do much of anything. So just power off.
 */
void
md_panic(void)
{
	md_poweroff();
}

/*
 * Function to get the size of installed physical RAM from the LAMEbus
 * controller.
 */
u_int32_t
mips_ramsize(void)
{
	return lamebus_ramsize();
}

/*
 * Interrupt dispatcher.
 */
void
mips_lamebus_interrupt(void)
{
	lamebus_interrupt(lamebus);
}
