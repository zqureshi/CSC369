#ifndef _LAMEBUS_LTRACE_H_
#define _LAMEBUS_LTRACE_H_

struct ltrace_softc {
	/* Initialized by lower-level attachment function */
	void *lt_busdata;
	u_int32_t lt_buspos;
};

/*
 * Functions provided for debug hacking:
 *   ltrace_on:    turns on the trace161 tracing flag CODE.
 *   ltrace_off:   turns off the trace161 tracing flag CODE.
 *   ltrace_debug: causes sys161/trace161 to print a message with CODE.
 *   ltrace_dump:  causes trace161 to do a complete state dump, tagged CODE.
 *
 * The flags for ltrace_on/off are the characters used to control
 * tracing on the trace161 command line. See the System/161 manual for
 * more information.
 *
 * ltrace_debug is for printing simple indications that a certain
 * piece of code has been reached, like one might use kprintf, except
 * that it is less invasive than kprintf. Think of it as setting the
 * value of a readout on the system's front panel. (In real life,
 * since computers don't have front panels with blinking lights any
 * more, people often use the speaker or the top left corner of the
 * screen for this purpose.)
 *
 * ltrace_dump dumps the entire system state and is primarily intended
 * for regression testing of System/161. It might or might not prove
 * useful for debugging as well.
 */
void ltrace_on(u_int32_t code);
void ltrace_off(u_int32_t code);
void ltrace_debug(u_int32_t code);
void ltrace_dump(u_int32_t code);

#endif /* _LAMEBUS_LTRACE_H_ */
