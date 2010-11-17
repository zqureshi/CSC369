#include <types.h>
#include <lib.h>
#include <machine/bus.h>
#include <machine/spl.h>
#include <machine/pcb.h>

/* Global that signals if we're presently in an interrupt handler. */
int in_interrupt;

/* 
 * General interrupt handler for mips.
 * "cause" is the contents of the c0_cause register.
 */

#define LAMEBUS_IRQ_BIT  0x00000400
#define LAMEBUS_NMI_BIT  0x00000800

void
mips_interrupt(u_int32_t cause)
{
	int old_in = in_interrupt;
	in_interrupt = 1;

	/* interrupts should be off */
	assert(curspl>0);

	if (cause & LAMEBUS_IRQ_BIT) {
		mips_lamebus_interrupt();
	}
	else if (cause & LAMEBUS_NMI_BIT) {
		panic("Received NMI\n");
	}
	else {
		panic("Unknown interrupt; cause register is %08x\n", cause);
	}

	in_interrupt = old_in;
}
