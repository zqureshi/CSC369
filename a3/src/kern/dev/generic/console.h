#ifndef _GENERIC_CONSOLE_H_
#define _GENERIC_CONSOLE_H_

/*
 * Device data for the hardware-independent system console.
 *
 * devdata, send, and sendpolled are provided by the underlying
 * device, and are to be initialized by the attach routine.
 */

struct con_softc {
	/* initialized by attach routine */
	void *cs_devdata;
	void (*cs_send)(void *devdata, int ch);
	void (*cs_sendpolled)(void *devdata, int ch);

	/* initialized by config routine */
	struct semaphore *cs_rsem;
	struct semaphore *cs_wsem;
	int cs_gotchar;
};

/*
 * Functions called by lower-level drivers
 */
void con_input(/*struct con_softc*/ void *cs, int ch);
void con_start(/*struct con_softc*/ void *cs);

/*
 * Functions called by higher-level code
 *
 * putch/getch - see <lib.h>
 */

#endif /* _GENERIC_CONSOLE_H_ */
