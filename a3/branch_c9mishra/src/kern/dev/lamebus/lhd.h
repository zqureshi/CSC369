#ifndef _LAMEBUS_LHD_H_
#define _LAMEBUS_LHD_H_

#include <dev.h>

/*
 * Our sector size
 */
#define LHD_SECTSIZE  512

/*
 * Hardware device data associated with lhd (LAMEbus hard disk)
 */
struct lhd_softc {
	/* Initialized by lower-level attach code */
	void *lh_busdata;		/* The bus we're on */ 
	u_int32_t lh_buspos;		/* Our slot on that bus */
	int lh_unit;			/* What number lhd we are */

	/*
	 * Initialized by config_lhd 
	 */

	void *lh_buf;			/* Pointer to on-card I/O buffer */
	int lh_result;			/* Result from I/O operation */
	struct semaphore *lh_clear;	/* Synchronization */
	struct semaphore *lh_done;

	struct device lh_dev;		/* VFS device structure */
};

/* Functions called by lower-level drivers */
void lhd_irq(/*struct lhd_softc*/ void *);	/* Interrupt handler */

#endif /* _LAMEBUS_LHD_H_ */
