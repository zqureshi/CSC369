#include <types.h>
#include <lib.h>
#include <machine/spl.h>
#include <machine/specialreg.h>

/*
 * Actual interrupt on/off functions.
 *
 * While the mips actually has on-chip interrupt priority masking, in
 * the interests of simplicity, we don't use it. Instead we use
 * coprocessor 0 register 12 (the system coprocessor "status"
 * register) bit 0, IEc, which is the global interrupt enable flag.
 * (IEc stands for interrupt-enable-current.)
 *
 * We use gcc inline assembly clauses to get at the status register.
 */

#define get_status(x) __asm volatile("mfc0 %0,$12" : "=r" (x))
#define set_status(x) __asm volatile("mtc0 %0,$12" :: "r" (x))

static
inline
void
interrupts_on(void)
{
	u_int32_t x;
	get_status(x);
	x |= CST_IEc;
	set_status(x);
}

static
inline
void
interrupts_off(void)
{
	u_int32_t x;
	get_status(x);
	x &= ~(u_int32_t) CST_IEc;
	set_status(x);
}

static
inline
void
interrupts_onoff(void)
{
	u_int32_t x, xon, xoff;
	get_status(x);
	xon = x | CST_IEc;
	xoff = x & ~(u_int32_t) CST_IEc;
	set_status(xon);
	set_status(xoff);
}

/*
 * spl manipulation.
 *
 * See arch/mips/include/spl.h for more information about what this is all
 * for.
 */

/* System starts out with interrupts off. */
int curspl = SPL_HIGH;

/* Set the spl level. */
int
splx(int newspl)
{
	int oldspl;
	
	/*
	 * We don't need to synchronize access to curspl, even if our
	 * accesses to it aren't atomic. If an interrupt occurs while
	 * we're accessing it, the interrupt will change the value,
	 * but the interrupt will set the value back the way it was
	 * before it finishes. (And that happens with interrupts
	 * disabled.) So we'll complete our half-finished operation
	 * without noticing. 
	 *
	 * And other threads can't interfere, because they'd have to
	 * run, and that would require an interrupt to occur first,
	 * and that interrupt would preserve the value of curspl we're
	 * working with.
	 *
	 * This would not be true if we were on a multiprocessor
	 * system, but we aren't, and the concept of a single curspl
	 * on a multiprocessor isn't especially valid anyway.
	 */


	/*
	 * Note: always explicitly set the interrupt state, to 
	 * minimize the consequences if we slip up accounting for
	 * interrupts being turned off by exceptions.
	 */
	if (newspl>0) {
		interrupts_off();
	}
	else if (newspl==0) {
		interrupts_on();
	}

	oldspl = curspl;
	curspl = newspl;

	return oldspl;
}

/* Set spl level to "high". */
int 
splhigh(void)
{
	return splx(SPL_HIGH);
}

int
spl0(void)
{
	return splx(0);
}

/*
 * Idle the processor until something happens.
 */

void 
cpu_idle(void)
{
	assert(curspl>0);
	/* 
	 * mips r2k/r3k has no idle instruction.
	 *
	 * However, to avoid completely overloading the computing cluster,
	 * we appropriate the WAIT instruction from later MIPS revisions.
	 * This goes into powersave mode until an interrupt is trying to 
	 * occur.
	 *
	 * Then switch interrupts on and off again, so we actually take
	 * the interrupt.
	 *
	 * Note that the precise behavior of this instruction in the
	 * System/161 simulator is partly guesswork, thanks to the
	 * specification being inadequate. So this code may well not
	 * work on a real mips r4k, never mind an r2k/r3k.
	 *
	 * Note that the assembler knows that "wait" is not a valid
	 * r2000/r3000 instruction and won't let us use it. Emit the
	 * bit pattern for the wait instruction as a long instead.
	 */

	/* __asm volatile("wait"); */
	__asm volatile(".long 0x42000020");

	interrupts_onoff();
}

/*
 * Halt the CPU permanently.
 */
void
cpu_halt(void)
{
	/*
	 * See notes in cpu_idle.
	 */

	interrupts_off();
	while (1) {
		/* __asm volatile("wait"); */
		__asm volatile(".long 0x42000020");
	}
}
