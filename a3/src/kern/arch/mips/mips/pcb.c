#include <types.h>
#include <lib.h>
#include <machine/pcb.h>
#include <machine/spl.h>   // for in_interrupt
#include <machine/switchframe.h>
#include <thread.h>

/* in switch.S */
extern void mips_switch(struct pcb *old, struct pcb *nu);

/* in threadstart.S */
extern void mips_threadstart(/* arguments are in unusual registers */);

u_int32_t curkstack;	/* curthread's kernel stack, for use on kernel entry */

/*
 * Function to initialize the pcb of the first (bootup) thread, which
 * is the thread that is presently running.
 *
 * Initialize pcb_badfaultfunc to NULL.
 *
 * We don't need to do anything else, since pcb_switchstack is always
 * overwritten at switch time anyway, and pcb_kstack is set at switch
 * time from the global curkstack, which was set by start.S.
 *
 * Nonetheless, set everything to workable values, just to be safe.
 */
void
md_initpcb0(struct pcb *pcb)
{
	pcb->pcb_switchstack = 0;
	pcb->pcb_kstack = curkstack;
	pcb->pcb_ininterrupt = 0;

	pcb->pcb_badfaultfunc = NULL;
}

/*
 * Function to initialize the pcb of a new thread, which is *not*
 * the one that is currently running.
 *
 * The new thread should, when it is run the first time, end up calling
 * mi_threadstart(data1, data2, func).
 *
 * We arrange for this by creating a phony switchframe for mips_switch()
 * to switch to. The only trouble is that the switchframe doesn't include
 * the argument registers a0-a3. So we store the arguments in the s* 
 * registers, and use a bit of asm (mips_threadstart) to move them and
 * then jump to mi_threadstart.
 */
void 
md_initpcb(struct pcb *pcb, char *stack, 
	   void *data1, unsigned long data2, 
	   void (*func)(void *, unsigned long))
{
	/*
	 * MIPS stacks grow down. What we get passed is just a hunk of
	 * memory. So get the other end of it.
	 */
	u_int32_t stacktop = ((u_int32_t)stack) + STACK_SIZE;

	/*
	 * Set up a switchframe on the top of the stack, and point to it.
	 */
	struct switchframe *sf = ((struct switchframe *) stacktop) - 1;

	/*
	 * pcb_badfaultfunc should start NULL.
	 *
	 * pcb_kstack should be what the stack pointer should be on 
	 * entry to the kernel, that is, the end with higher addresses.
	 *
	 * pcb_switchstack should be the address of the switchframe.
	 */
	pcb->pcb_badfaultfunc = NULL;
	pcb->pcb_kstack = stacktop;
	pcb->pcb_switchstack = (u_int32_t) sf;
	pcb->pcb_ininterrupt = 0;

	/*
	 * Zero out the switchframe.
	 */
	bzero(sf, sizeof(*sf));

	/*
	 * Now set the important parts: pass through the three arguments,
	 * and set the return address register to the place we want 
	 * execution to begin.
	 *
	 * Thus, when mips_switch does its "j ra", it will actually jump
	 * to mips_threadstart, which will move the arguments and jump to
	 * mi_threadstart().
	 *
	 * Note that this means that when we call mips_switch() in
	 * md_switch(), we may not come back out the same way in the
	 * next thread. (Though we will come back out the same way
	 * when we later come back to the same thread again.)
	 *
	 * This means that code at the bottom of md_switch, and
	 * mi_switch as well, may be reached fewer times than the top
	 * of md_switch. Thus, code should not be put there without
	 * exercising some caution.
	 */
	sf->sf_s0 = (u_int32_t)data1;
	sf->sf_s1 = (u_int32_t)data2;
	sf->sf_s2 = (u_int32_t)func;
	sf->sf_ra = (u_int32_t)mips_threadstart;
}

/*
 * Machine-dependent entry point for thread context switch.
 *
 * We save some globals (note: the MI code takes care of curthread)
 * whose values are really meant to be per-thread, and then call the
 * assembly switch function to do the real work.
 */
void
md_switch(struct pcb *old, struct pcb *nu)
{
	if (old==nu) {
		return;
	}
	/*
	 * Note: we don't need to switch curspl, because splhigh()
	 * should always be in effect when we get here and when we
	 * leave here.
	 */

	old->pcb_kstack = curkstack;
	old->pcb_ininterrupt = in_interrupt;

	curkstack = nu->pcb_kstack;
	in_interrupt = nu->pcb_ininterrupt;

	mips_switch(old, nu);

	/*
	 * Because new threads don't come back this way, and because
	 * the values of old and nu are not what one would necessarily
	 * expect after mips_switch, code should not be put here.
	 */
}
