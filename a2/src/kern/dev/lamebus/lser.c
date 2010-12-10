#include <types.h>
#include <lib.h>
#include <machine/spl.h>
#include <machine/bus.h>
#include <lamebus/lser.h>
#include "autoconf.h"

/* Registers (offsets within slot) */
#define LSER_REG_CHAR  0     /* Character in/out */
#define LSER_REG_WIRQ  4     /* Write interrupt status */
#define LSER_REG_RIRQ  8     /* Read interrupt status */

/* Bits in the IRQ registers */
#define LSER_IRQ_ENABLE  1
#define LSER_IRQ_ACTIVE  2

void
lser_irq(void *vsc)
{
	struct lser_softc *sc = vsc;
	u_int32_t x;
	int clear_to_write=0;
	int got_a_read=0;
	u_int32_t ch = 0;

	assert(curspl>0);

	x = bus_read_register(sc->ls_busdata, sc->ls_buspos, LSER_REG_WIRQ);
	if (x & LSER_IRQ_ACTIVE) {
		x = LSER_IRQ_ENABLE;
		sc->ls_wbusy = 0;
		clear_to_write = 1;
		bus_write_register(sc->ls_busdata, sc->ls_buspos,
				   LSER_REG_WIRQ, x);
	}

	x = bus_read_register(sc->ls_busdata, sc->ls_buspos, LSER_REG_RIRQ);
	if (x & LSER_IRQ_ACTIVE) {
		x = LSER_IRQ_ENABLE;
		ch = bus_read_register(sc->ls_busdata, sc->ls_buspos,
				       LSER_REG_CHAR);
		got_a_read = 1;
		bus_write_register(sc->ls_busdata, sc->ls_buspos, 
				   LSER_REG_RIRQ, x);
	}

	if (clear_to_write && sc->ls_start != NULL) {
		sc->ls_start(sc->ls_devdata);
	}
	if (got_a_read && sc->ls_input != NULL) {
		sc->ls_input(sc->ls_devdata, ch);
	}
}

void
lser_write(void *vls, int ch)
{
	struct lser_softc *ls = vls;
	int spl = splhigh();

	if (ls->ls_wbusy) {
		/*
		 * We're not clear to write.
		 *
		 * This should not happen. It's the job of the driver
		 * attached to us to not write until we call
		 * ls->ls_start.
		 *
		 * (Note: if we're the console, the panic will go to
		 * lser_writepolled for printing, because spl is high;
		 * it won't recurse.)
		 */
		panic("lser: Not clear to write\n");
	}
	ls->ls_wbusy = 1;

	bus_write_register(ls->ls_busdata, ls->ls_buspos, LSER_REG_CHAR, ch);

	splx(spl);
}

static
void
lser_spin_until_write(struct lser_softc *sc)
{
	u_int32_t val;
	assert(curspl>0);
	do {
		val = bus_read_register(sc->ls_busdata, sc->ls_buspos,
					LSER_REG_WIRQ);
	}
	while ((val & LSER_IRQ_ACTIVE)==0);
}

void
lser_writepolled(void *vsc, int ch)
{
	struct lser_softc *sc = vsc;
	int spl = splhigh();
	int irqpending=0;

	if (sc->ls_wbusy) {
		irqpending = 1;
		lser_spin_until_write(sc);
		/* Clear the ready condition */
		bus_write_register(sc->ls_busdata, sc->ls_buspos,
				   LSER_REG_WIRQ, LSER_IRQ_ENABLE);
	}

	/* Send the character. */
	bus_write_register(sc->ls_busdata, sc->ls_buspos, LSER_REG_CHAR, ch);

	/* Wait until it's done. */
	lser_spin_until_write(sc);

	/*
	 * If there wasn't already an IRQ pending, clear the ready condition.
	 * But if there was, leave the ready condition, so we get to the 
	 * interrupt handler in due course.
	 */
	if (!irqpending) {
		bus_write_register(sc->ls_busdata, sc->ls_buspos,
				   LSER_REG_WIRQ, LSER_IRQ_ENABLE);
	}

	splx(spl);
}

int
config_lser(struct lser_softc *sc, int lserno)
{
	(void)lserno;

	/*
	 * Enable interrupting.
	 */

	sc->ls_wbusy = 0;

	bus_write_register(sc->ls_busdata, sc->ls_buspos,
			   LSER_REG_RIRQ, LSER_IRQ_ENABLE);
	bus_write_register(sc->ls_busdata, sc->ls_buspos,
			   LSER_REG_WIRQ, LSER_IRQ_ENABLE);

	return 0;
}

