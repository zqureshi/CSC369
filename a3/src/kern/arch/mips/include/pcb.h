/*
 * Process Control Block: machine-dependent part of thread
 */

#ifndef _MACHINE_PCB_H_
#define _MACHINE_PCB_H_

#include <machine/setjmp.h>

/* Size of kernel stacks (bytes) */
#define STACK_SIZE  4096

/* Mask for extracting the stack base address of a kernel stack pointer */
#define STACK_MASK  0xfffff000

/* Macro to test if two addresses are on the same kernel stack */
#define SAME_STACK(p1,p2)	(((p1)&STACK_MASK)==((p2)&STACK_MASK))

/* Function pointer type for pcb_badfaultfunc: void, returning void */
typedef void (*pcb_faultfunc)(void);

/*
 * Note: pcb_kstack and pcb_ininterrupt are saved values of the
 * globals "curkstack" and "in_interrupt", respectively. Thus, when a
 * thread is presently running they should not be used - the globals
 * should be used instead.
 *
 * Said variables are globals because they're used in assembly code
 * and it's much easier to access globals in assembly than to try to
 * setting up a mechanism for converting C structure offsets to
 * symbols that the assembler can make use of. Obviously, the latter
 * is *possible*, and structure accesses are in fact done that way in
 * BSD.
 *
 * Note that pcb_switchstack MUST BE THE FIRST THING IN THE PCB or
 * switch.S will have a coronary.
 */
struct pcb {
	u_int32_t pcb_switchstack;  // stack saved during context switch
	u_int32_t pcb_kstack;	    // stack to load on entry to kernel
	u_int32_t pcb_ininterrupt;  // are we in an interrupt handler?

	pcb_faultfunc pcb_badfaultfunc; // recovery for fatal kernel traps
	jmp_buf pcb_copyjmp;            // jump area used by copyin/out etc.
};


/*
 * Machine-dependent thread functions used by the machine-independent
 * thread code.
 */

/* Initialize the pcb of the first (bootup) thread */
void md_initpcb0(struct pcb *);

/*
 * Initialize the pcb of a newly created thread. The newly created
 * thread, when it first runs, should call mi_threadstart, to which
 * data1, data2, and func are arguments.
 */
void md_initpcb(struct pcb *, char *stack, void *data1, unsigned long data2,
		void (*func)(void *, unsigned long));

/*
 * Enter user mode. Does not return.
 *
 * Performs the necessary initialization so that the user program will
 * get the arguments supplied in argc/argv (note that argv must be a
 * user-level address), and begin executing at the specified entry
 * point. The stack pointer is initialized from the stackptr
 * argument. Note that passing argc/argv may use additional stack
 * space on some other platforms (but not mips).
 */
void md_usermode(int argc, userptr_t argv, vaddr_t stackptr,
		 vaddr_t entrypoint);

/*
 * The various ways to shut down the system. (These are very low-level
 * and should generally not be called directly - md_poweroff, for
 * instance, unceremoniously turns the power off without doing
 * anything else.)
 */
void md_poweroff(void);
void md_reboot(void);
void md_halt(void);
void md_panic(void);

/*
 * Various MIPS-specific functions.
 */

/* general interrupt handler */
void mips_interrupt(u_int32_t cause_register);

/* system call dispatcher */
struct trapframe;
void mips_syscall(struct trapframe *tf);

/* function to look up the size of physical RAM (returns count in bytes) */
u_int32_t mips_ramsize(void);


#endif /* _MACHINE_PCB_H_ */
