#ifndef _LAMEBUS_LSER_H_
#define _LAMEBUS_LSER_H_

struct lser_softc {
	/* Initialized by config function; synchronized with spl */
	volatile int ls_wbusy;     /* true if write in progress */

	/* Initialized by lower-level attachment function */
	void *ls_busdata;
	u_int32_t ls_buspos;

	/* Initialized by higher-level attachment function */
	void *ls_devdata;
	void (*ls_start)(void *devdata);
	void (*ls_input)(void *devdata, int ch);
};

/* Functions called by lower-level drivers */
void lser_irq(/*struct lser_softc*/ void *sc);

/* Functions called by higher-level drivers */
void lser_write(/*struct lser_softc*/ void *sc, int ch);
void lser_writepolled(/*struct lser_softc*/ void *sc, int ch);

#endif /* _LAMEBUS_LSER_H_ */
