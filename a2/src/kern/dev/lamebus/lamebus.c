/*
 * Machine-independent LAMEbus code.
 */

#include <types.h>
#include <lib.h>
#include <machine/spl.h>
#include <lamebus/lamebus.h>

/* Register offsets within each config region */
#define CFGREG_VID   0    /* Vendor ID */
#define CFGREG_DID   4    /* Device ID */
#define CFGREG_DRL   8    /* Device Revision Level */

/* LAMEbus controller private registers (offsets within its config region) */
#define CTLREG_RAMSZ    0x200
#define CTLREG_IRQS     0x204
#define CTLREG_PWR      0x208


/*
 * Read a config register for the given slot.
 */
static
inline
u_int32_t
read_cfg_register(struct lamebus_softc *lb, int slot, u_int32_t offset)
{
	/* Note that lb might be NULL on some platforms in some contexts. */
	offset += LB_CONFIG_SIZE*slot;
	return lamebus_read_register(lb, LB_CONTROLLER_SLOT, offset);
}

/*
 * Write a config register for a given slot.
 */
static
inline
void
write_cfg_register(struct lamebus_softc *lb, int slot, u_int32_t offset,
		   u_int32_t val)
{
	offset += LB_CONFIG_SIZE*slot;
	lamebus_write_register(lb, LB_CONTROLLER_SLOT, offset, val);
}

/*
 * Read one of the bus controller's registers.
 */
static
inline
u_int32_t
read_ctl_register(struct lamebus_softc *lb, u_int32_t offset)
{
	/* Note that lb might be NULL on some platforms in some contexts. */
	return read_cfg_register(lb, LB_CONTROLLER_SLOT, offset);
}

/*
 * Write one of the bus controller's registers.
 */
static
inline
void
write_ctl_register(struct lamebus_softc *lb, u_int32_t offset, u_int32_t val)
{
	write_cfg_register(lb, LB_CONTROLLER_SLOT, offset, val);
}

/*
 * Probe function.
 *
 * Given a LAMEbus, look for a device that's not already been marked
 * in use, has the specified IDs, and has a device revision level in
 * the specified range (which is inclusive on both ends.)
 *
 * Returns the slot number found (0-31) or -1 if nothing suitable was
 * found.
 */

int
lamebus_probe(struct lamebus_softc *sc,
	      u_int32_t vendorid, u_int32_t deviceid,
	      u_int32_t lowver, u_int32_t highver)
{
	int slot;
	u_int32_t val;
	int spl;

	/*
	 * Because the slot information in sc is used when dispatching
	 * interrupts, disable interrupts while working with it.
	 */

	spl = splhigh();

	for (slot=0; slot<LB_NSLOTS; slot++) {
		if (sc->ls_slotsinuse & (1<<slot)) {
			/* Slot already in use; skip */
			continue;
		}

		val = read_cfg_register(sc, slot, CFGREG_VID);
		if (val!=vendorid) {
			/* Wrong vendor id */
			continue;
		}

		val = read_cfg_register(sc, slot, CFGREG_DID);
		if (val != deviceid) {
			/* Wrong device id */
			continue;
		}

		val = read_cfg_register(sc, slot, CFGREG_DRL);
		if (val < lowver || val > highver) {
			/* Unsupported device revision */
			continue;
		}

		/* Found something */

		splx(spl);
		return slot;
	}

	/* Found nothing */

	splx(spl);
	return -1;
}

/*
 * Mark that a slot is in use.
 * This prevents the probe routine from returning the same device over
 * and over again.
 */
void
lamebus_mark(struct lamebus_softc *sc, int slot)
{
	int spl;

	u_int32_t mask = ((u_int32_t)1) << slot;
	assert(slot>=0 && slot < LB_NSLOTS);

	spl = splhigh();

	if ((sc->ls_slotsinuse & mask)!=0) {
		panic("lamebus_mark: slot %d already in use\n", slot);
	}

	sc->ls_slotsinuse |= mask;

	splx(spl);
}

/*
 * Mark that a slot is no longer in use.
 */
void
lamebus_unmark(struct lamebus_softc *sc, int slot)
{
	int spl;

	u_int32_t mask = ((u_int32_t)1) << slot;
	assert(slot>=0 && slot < LB_NSLOTS);

	spl = splhigh();

	if ((sc->ls_slotsinuse & mask)==0) {
		panic("lamebus_mark: slot %d not marked in use\n", slot);
	}

	sc->ls_slotsinuse &= ~mask;

	splx(spl);
}

/*
 * Register a function (and a device context pointer) to be called
 * when a particular slot signals an interrupt.
 */
void
lamebus_attach_interrupt(struct lamebus_softc *sc, int slot,
			 void *devdata,
			 void (*irqfunc)(void *devdata))
{
	int spl;

	u_int32_t mask = ((u_int32_t)1) << slot;
	assert(slot>=0 && slot < LB_NSLOTS);

	spl = splhigh();

	if ((sc->ls_slotsinuse & mask)==0) {
		panic("lamebus_attach_interrupt: slot %d not marked in use\n",
		      slot);
	}

	assert(sc->ls_devdata[slot]==NULL);
	assert(sc->ls_irqfuncs[slot]==NULL);

	sc->ls_devdata[slot] = devdata;
	sc->ls_irqfuncs[slot] = irqfunc;
	
	splx(spl);
}

/*
 * Unregister a function that was being called when a particular slot
 * signaled an interrupt.
 */
void
lamebus_detach_interrupt(struct lamebus_softc *sc, int slot)
{
	int spl;

	u_int32_t mask = ((u_int32_t)1) << slot;
	assert(slot>=0 && slot < LB_NSLOTS);

	spl = splhigh();

	if ((sc->ls_slotsinuse & mask)==0) {
		panic("lamebus_detach_interrupt: slot %d not marked in use\n",
		      slot);
	}

	assert(sc->ls_irqfuncs[slot]!=NULL);

	sc->ls_devdata[slot] = NULL;
	sc->ls_irqfuncs[slot] = NULL;
	
	splx(spl);
}


/*
 * LAMEbus interrupt handling function. (Machine-independent!)
 */
void
lamebus_interrupt(struct lamebus_softc *lamebus)
{
	/*
	 * Note that despite the fact that "spl" stands for "set
	 * priority level", we don't actually support interrupt
	 * priorities. When an interrupt happens, we look through the
	 * slots to find the first interrupting device and call its
	 * interrupt routine, no matter what that device is.
	 *
	 * Note that the entire LAMEbus uses only one on-cpu interrupt line. 
	 * Thus, we do not use any on-cpu interrupt priority system either.
	 */

	int slot;
	u_int32_t mask;
	u_int32_t irqs;

	/* For keeping track of how many bogus things happen in a row. */
	static int duds=0;
	int duds_this_time=0;

	/* spl had better be raised. */
	assert(curspl>0);

	/* and we better have a valid bus instance. */
	assert(lamebus!=NULL);

	/*
	 * Read the LAMEbus controller register that tells us which
	 * slots are asserting an interrupt condition.
	 */
	irqs = read_ctl_register(lamebus, CTLREG_IRQS);

	if (irqs==0) {
		/*
		 * Huh? None of them? Must be a glitch.
		 */
		kprintf("lamebus: stray interrupt\n");
		duds++;
		duds_this_time++;

		/*
		 * We could just return now, but instead we'll
		 * continue ahead. Because irqs==0, nothing in the
		 * loop will execute, and passing through it gets us
		 * to the code that checks how many duds we've
		 * seen. This is important, because we just might get
		 * a stray interrupt that latches itself on. If that
		 * happens, we're pretty much toast, but it's better
		 * to panic and hopefully reset the system than to
		 * loop forever printing "stray interrupt".
		 */
		return;
	}

	/*
	 * Go through the bits in the value we got back to see which
	 * ones are set.
	 */

	for (mask=1, slot=0; slot<LB_NSLOTS; mask<<=1, slot++) {
		if ((irqs & mask)==0) {
			/* Nope. */
			continue;
		}

		/*
		 * This slot is signalling an interrupt.
		 */
			
		if ((lamebus->ls_slotsinuse & mask)==0) {
			/*
			 * No device driver is using this slot.
			 */
			duds++;
			duds_this_time++;
			continue;
		}

		if (lamebus->ls_irqfuncs[slot]==NULL) {
			/*
			 * The device driver hasn't installed an interrupt
			 * handler.
			 */
			duds++;
			duds_this_time++;
			continue;
		}

		/*
		 * Call the interrupt handler.
		 * Note that interrupts are off here so it's ok to access
		 * the global lamebus structure.
		 */
		lamebus->ls_irqfuncs[slot](lamebus->ls_devdata[slot]);

		/*
		 * Reload the mask of pending IRQs - if we just called
		 * hardclock, it might have changed under us.
		 */

		irqs = read_ctl_register(lamebus, CTLREG_IRQS);
	}


	/*
	 * If we get interrupts for a slot with no driver or no
	 * interrupt handler, it's fairly serious. Because LAMEbus
	 * uses level-triggered interrupts, if we don't shut off the
	 * condition, we'll keep getting interrupted continuously and
	 * the system will make no progress. But we don't know how to
	 * do that if there's no driver or no interrupt handler.
	 *
	 * So, if we get too many dud interrupts, panic, since it's 
	 * better to panic and reset than to hang.
	 *
	 * If we get through here without seeing any duds this time,
	 * the condition, whatever it was, has gone away. It might be
	 * some stupid device we don't have a driver for, or it might
	 * have been an electrical transient. In any case, warn and
	 * clear the dud count.
	 */

	if (duds_this_time==0 && duds>0) {
		kprintf("lamebus: %d dud interrupts\n", duds);
		duds = 0;
	}

	if (duds > 10000) {
		panic("lamebus: too many (%d) dud interrupts\n", duds);
	}
}

/*
 * Have the bus controller power the system off.
 */
void
lamebus_poweroff(struct lamebus_softc *lamebus)
{
	/*
	 * Write 0 to the power register to shut the system off.
	 */

	splhigh();
	write_ctl_register(lamebus, CTLREG_PWR, 0);

	/* The power doesn't go off instantly... halt the cpu. */
	cpu_halt();
}

/*
 * Ask the bus controller how much memory we have.
 */
u_int32_t
lamebus_ramsize(void)
{
	/*
	 * Note that this has to work before bus initialization.
	 * On machines where lamebus_read_register doesn't work
	 * before bus initialization, this function can't be used
	 * for initial RAM size lookup.
	 */

	return read_ctl_register(NULL, CTLREG_RAMSZ);
}

/*
 * Initial setup.
 * Should be called from machdep_dev_bootstrap().
 */
struct lamebus_softc *
lamebus_init(void)
{
	struct lamebus_softc *lamebus;
	int i;

	/* Allocate space for lamebus data */
	lamebus = kmalloc(sizeof(struct lamebus_softc));
	if (lamebus==NULL) {
		panic("lamebus_init: Out of memory\n");
	}

	/*
	 * Initialize the LAMEbus data structure.
	 */
	lamebus->ls_slotsinuse = 1 << LB_CONTROLLER_SLOT;

	for (i=0; i<LB_NSLOTS; i++) {
		lamebus->ls_devdata[i] = NULL;
		lamebus->ls_irqfuncs[i] = NULL;
	}

	return lamebus;
}
