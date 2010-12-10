#ifndef _LAMEBUS_LSCREEN_H_
#define _LAMEBUS_LSCREEN_H_

/*
 * Hardware device data for memory-mapped fullscreen text console.
 */
struct lscreen_softc {
	/* Initialized by config function; synchronized with spl */
	unsigned ls_width, ls_height; // screen size
	unsigned ls_cx, ls_cy;        // cursor position
	char *ls_screen;              // memory-mapped screen buffer

	/* Initialized by lower-level attachment function */
	void *ls_busdata;		// bus we're on
	u_int32_t ls_buspos;		// position on that bus

	/* Initialized by higher-level attachment function */
	void *ls_devdata;			// data and functions for
	void (*ls_start)(void *devdata);	// upper device (perhaps
	void (*ls_input)(void *devdata, int ch); // console)
};

/* Functions called by lower-level drivers */
void lscreen_irq(/*struct lser_softc*/ void *sc);  // interrupt handler

/* Functions called by higher-level drivers */
void lscreen_write(/*struct lser_softc*/ void *sc, int ch); // output function

#endif /* _LAMEBUS_LSCREEN_H_ */
