#include <types.h>
#include <lib.h>
#include <machine/bus.h>
#include <lamebus/ltrace.h>
#include "autoconf.h"

/* Registers (offsets within slot) */
#define LTRACE_REG_TRON    0
#define LTRACE_REG_TROFF   4
#define LTRACE_REG_DEBUG   8
#define LTRACE_REG_DUMP    12

static struct ltrace_softc *the_trace;

void
ltrace_on(u_int32_t code)
{
	if (the_trace != NULL) {
		bus_write_register(the_trace->lt_busdata, the_trace->lt_buspos,
				   LTRACE_REG_TRON, code);
	}
}

void
ltrace_off(u_int32_t code)
{
	if (the_trace != NULL) {
		bus_write_register(the_trace->lt_busdata, the_trace->lt_buspos,
				   LTRACE_REG_TROFF, code);
	}
}

void
ltrace_debug(u_int32_t code)
{
	if (the_trace != NULL) {
		bus_write_register(the_trace->lt_busdata, the_trace->lt_buspos,
				   LTRACE_REG_DEBUG, code);
	}
}

void
ltrace_dump(u_int32_t code)
{
	if (the_trace != NULL) {
		bus_write_register(the_trace->lt_busdata, the_trace->lt_buspos,
				   LTRACE_REG_DUMP, code);
	}
}

int
config_ltrace(struct ltrace_softc *sc, int ltraceno)
{
	(void)ltraceno;
	the_trace = sc;
	return 0;
}
