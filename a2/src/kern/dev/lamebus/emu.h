#ifndef _LAMEBUS_EMU_H_
#define _LAMEBUS_EMU_H_

#define EMU_MAXIO       16384
#define EMU_ROOTHANDLE  0

/*
 * The per-device data used by the emufs device driver.
 * (Note that this is only a small portion of its actual data;
 * all the filesystem stuff goes elsewhere.
 */

struct emu_softc {
	/* Initialized by lower-level attach code */
	void *e_busdata;
	u_int32_t e_buspos;
	int e_unit;

	/* Initialized by config_emu() */
	struct lock *e_lock;
	struct semaphore *e_sem;
	void *e_iobuf;

	/* Written by the interrupt handler */
	u_int32_t e_result;
};

/* Functions called by lower-level drivers */
void emu_irq(/*struct emu_softc*/ void *);

#endif /* _LAMEBUS_EMU_H_ */
