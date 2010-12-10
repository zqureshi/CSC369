#include <types.h>
#include <lib.h>
#include <machine/trapframe.h>
#include <machine/specialreg.h>
#include <machine/pcb.h>
#include <machine/spl.h>
#include <vm.h>
#include <thread.h>
#include <curthread.h>

extern u_int32_t curkstack;

/* in exception.S */
extern void asm_usermode(struct trapframe *tf);

/* Names for trap codes */
#define NTRAPCODES 13
static const char *const trapcodenames[NTRAPCODES] = {
	"Interrupt",
	"TLB modify trap",
	"TLB miss on load",
	"TLB miss on store",
	"Address error on load",
	"Address error on store",
	"Bus error on code",
	"Bus error on data",
	"System call",
	"Break instruction",
	"Illegal instruction",
	"Coprocessor unusable",
	"Arithmetic overflow",
};

/*
 * Function called when user-level code hits a fatal fault.
 */
static
void
kill_curthread(u_int32_t epc, unsigned code, u_int32_t vaddr)
{
	assert(code<NTRAPCODES);
	kprintf("Fatal user mode trap %u (%s, epc 0x%x, vaddr 0x%x)\n",
		code, trapcodenames[code], epc, vaddr);

	/*
	 * You will probably want to change this.
	 */
	panic("I don't know how to handle this\n");
}

/*
 * General trap (exception) handling function for mips.
 * This is called by the assembly-language exception handler once
 * the trapframe has been set up.
 */
void
mips_trap(struct trapframe *tf)
{
	u_int32_t code, isutlb, iskern;
	int savespl;

	/* The trap frame is supposed to be 37 registers long. */
	assert(sizeof(struct trapframe)==(37*4));

	/* Save the value of curspl, which belongs to the old context. */
	savespl = curspl;

	/* Right now, interrupts should be off. */
	curspl = SPL_HIGH;

	/*
	 * Extract the exception code info from the register fields.
	 */
	code = (tf->tf_cause & CCA_CODE) >> CCA_CODESHIFT;
	isutlb = (tf->tf_cause & CCA_UTLB);
	iskern = (tf->tf_status & CST_KUp)==0;

	assert(code<NTRAPCODES);

	/* Make sure we haven't run off our stack */
	if (curthread != NULL && curthread->t_stack != NULL) {
		assert((vaddr_t)tf > (vaddr_t)curthread->t_stack);
		assert((vaddr_t)tf < (vaddr_t)(curthread->t_stack+STACK_SIZE));
	}

	/* Interrupt? Call the interrupt handler and return. */
	if (code == EX_IRQ) {
		mips_interrupt(tf->tf_cause);
		goto done;
	}

	/*
	 * While we're in the kernel, and not actually handling an
	 * interrupt, leave spl where it was in the previous context,
	 * which is probably low (interrupts on).
	 */
	splx(savespl);

	/* Syscall? Call the syscall handler and return. */
	if (code == EX_SYS) {
		/* Interrupts should have been on while in user mode. */
		assert(curspl==0);

		DEBUG(DB_SYSCALL, "syscall: #%d, args %x %x %x %x\n", 
		      tf->tf_v0, tf->tf_a0, tf->tf_a1, tf->tf_a2, tf->tf_a3);

		mips_syscall(tf);
		goto done;
	}

	/*
	 * Ok, it wasn't any of the really easy cases.
	 * Call vm_fault on the TLB exceptions.
	 * Panic on the bus error exceptions.
	 */
	switch (code) {
	case EX_MOD:
		if (vm_fault(VM_FAULT_READONLY, tf->tf_vaddr)==0) {
			goto done;
		}
		break;
	case EX_TLBL:
		if (vm_fault(VM_FAULT_READ, tf->tf_vaddr)==0) {
			goto done;
		}
		break;
	case EX_TLBS:
		if (vm_fault(VM_FAULT_WRITE, tf->tf_vaddr)==0) {
			goto done;
		}
		break;
	case EX_IBE:
	case EX_DBE:
		/*
		 * This means you loaded invalid TLB entries, or 
		 * touched invalid parts of the direct-mapped 
		 * segments. These are serious kernel errors, so
		 * panic.
		 * 
		 * The MIPS won't even tell you what invalid address
		 * caused the bus error.
		 */
		panic("Bus error exception, PC=0x%x\n", tf->tf_epc);
		break;
	}

	/*
	 * If we get to this point, it's a fatal fault - either it's
	 * one of the other exceptions, like illegal instruction, or
	 * it was a page fault we couldn't handle.
	 */

	if (!iskern) {
		/*
		 * Fatal fault in user mode.
		 * Kill the current user process.
		 */
		kill_curthread(tf->tf_epc, code, tf->tf_vaddr);
		goto done;
	}

	/*
	 * Fatal fault in kernel mode.
	 *
	 * If pcb_badfaultfunc is set, we do not panic; badfaultfunc is
	 * set by copyin/copyout and related functions to signify that
	 * the addresses they're accessing are userlevel-supplied and
	 * not trustable. What we actually want to do is resume
	 * execution at the function pointed to by badfaultfunc. That's 
	 * going to be "copyfail" (see copyinout.c), which longjmps 
	 * back to copyin/copyout or wherever and returns EFAULT.
	 *
	 * Note that we do not just *call* this function, because that
	 * won't necessarily do anything. We want the control flow
	 * that is currently executing in copyin (or whichever), and
	 * is stopped while we process the exception, to *teleport* to
	 * copyerr.
	 *
	 * This is accomplished by changing tf->tf_epc and returning
	 * from the exception handler.
	 */

	if (curthread != NULL && curthread->t_pcb.pcb_badfaultfunc != NULL) {
		tf->tf_epc = (vaddr_t) curthread->t_pcb.pcb_badfaultfunc;
		goto done;
	}

	/*
	 * Really fatal kernel-mode fault.
	 */

	kprintf("panic: Fatal exception %u (%s) in kernel mode\n", code,
		trapcodenames[code]);
	kprintf("panic: EPC 0x%x, exception vaddr 0x%x\n", 
		tf->tf_epc, tf->tf_vaddr);

	panic("I can't handle this... I think I'll just die now...\n");

 done:
	/* Make sure interrupts are off */
	splhigh();

	/*
	 * Restore previous context's curspl value.
	 *
	 * The previous context's actual interrupt status flag will
	 * be restored by the RFE instruction at the end of trap return.
	 */
	curspl = savespl;

	/*
	 * This assertion will fail if either
	 *   (1) curkstack is corrupted, or
	 *   (2) the trap frame is somehow on the wrong kernel stack.
	 *
	 * If curkstack is corrupted, the next trap back to the kernel
	 * will (most likely) hang the system, so it's better to find
	 * out now.
	 */
	assert(SAME_STACK(curkstack-1, (vaddr_t)tf));
}

/*
 * Functions for entering user mode.
 *
 * This should not be used by threads returning from traps - they
 * should just return from mips_trap(). It should be used by threads
 * entering user mode for the first time - whether the child thread in
 * a fork(), or into a brand-new address space after exec(), or when
 * starting the first userlevel program.
 *
 * mips_usermode is the common code. It should not be called outside
 * the mips port.
 *
 * md_usermode is meant for use in exec and equivalent.
 * md_forkentry, in syscall.c, is meant for use in fork.
 */
void
mips_usermode(struct trapframe *tf)
{

	/*
	 * Interrupts should be off within the kernel while entering
	 * usermode. However, while in usermode, interrupts should be
	 * on. To interact properly with the spl-handling logic above,
	 * we call splhigh() to disable interrupts, but set curspl
	 * explicitly to 0.
	 */
	splhigh();
	curspl = 0;

	/*
	 * This assertion will fail if either
	 *   (1) curkstack is corrupted, or
	 *   (2) the trap frame is not on our own kernel stack.
	 *
	 * If curkstack is corrupted, the next trap back to the kernel
	 * will (most likely) hang the system, so it's better to find
	 * out now.
	 *
	 * It's necessary for the trap frame used here to be on the
	 * current thread's own stack. It cannot correctly be on either
	 * another thread's stack or in the kernel heap. (Why?)
	 */
	assert(SAME_STACK(curkstack-1, (vaddr_t)tf));

	/*
	 * This actually does it. See exception.S.
	 */
	asm_usermode(tf);
}

/*
 * md_usermode: go to user mode after loading an executable.
 *
 * Works by creating an ersatz trapframe and jumping into the middle
 * of the exception return code.
 */
void
md_usermode(int argc, userptr_t argv, vaddr_t stack, vaddr_t entry)
{
	struct trapframe tf;

	bzero(&tf, sizeof(tf));

	tf.tf_status = CST_IRQMASK | CST_IEp | CST_KUp;
	tf.tf_epc = entry;
	tf.tf_a0 = argc;
	tf.tf_a1 = (vaddr_t)argv;
	tf.tf_sp = stack;

	mips_usermode(&tf);
}
