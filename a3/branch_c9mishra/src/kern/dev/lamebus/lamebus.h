#ifndef _LAMEBUS_H_
#define _LAMEBUS_H_

/*
 * Machine-independent LAMEbus definitions.
 */


/* Vendors */
#define LB_VENDOR_CS161      1

/* CS161 devices */
#define LBCS161_BUSCTL       1
#define LBCS161_TIMER        2
#define LBCS161_DISK         3
#define LBCS161_SERIAL       4
#define LBCS161_SCREEN       5
#define LBCS161_NET          6
#define LBCS161_EMUFS        7
#define LBCS161_TRACE        8
#define LBCS161_RANDOM       9

/* LAMEbus controller always goes in slot 31 */
#define LB_CONTROLLER_SLOT   31

/* Number of slots */
#define LB_NSLOTS            32

/* LAMEbus controller per-slot config space */
#define LB_CONFIG_SIZE       1024

/* LAMEbus mapping size per slot */
#define LB_SLOT_SIZE         65536

/* Pointer to kind of function called on interrupt */
typedef void (*lb_irqfunc)(void *devdata);

/*
 * Driver data
 */
struct lamebus_softc {
	/* Accessed from interrupts; synchronized via spl */ 
	u_int32_t    ls_slotsinuse;
	void        *ls_devdata[LB_NSLOTS];
	lb_irqfunc   ls_irqfuncs[LB_NSLOTS];
};

/*
 * Allocate and set up a lamebus_softc for the system.
 */
struct lamebus_softc *lamebus_init(void);

/*
 * Look for a not-in-use slot containing a device whose vendor and device 
 * ids match those provided, and whose version is in the range between
 * lowver and highver, inclusive.
 *
 * Returns a slot number (0-31) or -1 if no such device is found.
 */
int lamebus_probe(struct lamebus_softc *, 
		  u_int32_t vendorid, u_int32_t deviceid, 
		  u_int32_t lowver, u_int32_t highver);

/*
 * Mark a slot in-use (that is, has a device driver attached to it),
 * or unmark it. It is a fatal error to mark a slot that is already 
 * in use, or unmark a slot that is not in use.
 */
void lamebus_mark(struct lamebus_softc *, int slot);
void lamebus_unmark(struct lamebus_softc *, int slot);

/*
 * Attach to an interrupt.
 */
void lamebus_attach_interrupt(struct lamebus_softc *, int slot,
			      void *devdata, 
			      void (*irqfunc)(void *devdata));
/*
 * Detach from interrupt.
 */
void lamebus_detach_interrupt(struct lamebus_softc *, int slot);

/*
 * Function to call to handle a LAMEbus interrupt.
 */
void lamebus_interrupt(struct lamebus_softc *);

/*
 * Have the LAMEbus controller power the system off.
 */
void lamebus_poweroff(struct lamebus_softc *);

/*
 * Ask the bus controller how much memory we have.
 */
u_int32_t lamebus_ramsize(void);

/*
 * Read/write 32-bit register at offset OFFSET within slot SLOT.
 * (Machine dependent.)
 */
u_int32_t lamebus_read_register(struct lamebus_softc *, int slot,
				u_int32_t offset);
void lamebus_write_register(struct lamebus_softc *, int slot,
			    u_int32_t offset, u_int32_t val);

/*
 * Map a buffer that starts at offset OFFSET within slot SLOT.
 */
void *lamebus_map_area(struct lamebus_softc *, int slot,
		       u_int32_t offset);


#endif /* _LAMEBUS_H_ */
