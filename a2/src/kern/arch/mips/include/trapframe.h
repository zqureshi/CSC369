#ifndef _MIPS_TRAPFRAME_H_
#define _MIPS_TRAPFRAME_H_

/*
 * Structure describing what is saved on the stack during entry to
 * the exception handler.
 *
 * This must agree with the code in exception.S.
 */

struct trapframe {
	u_int32_t tf_vaddr;	/* coprocessor 0 vaddr register */
	u_int32_t tf_status;	/* coprocessor 0 status register */
	u_int32_t tf_cause;	/* coprocessor 0 cause register */
	u_int32_t tf_lo;
	u_int32_t tf_hi;
	u_int32_t tf_ra;	/* Saved register 31 */
	u_int32_t tf_at;	/* Saved register 1 (AT) */
	u_int32_t tf_v0;	/* Saved register 2 (v0) */
	u_int32_t tf_v1;	/* etc. */
	u_int32_t tf_a0;
	u_int32_t tf_a1;
	u_int32_t tf_a2;
	u_int32_t tf_a3;
	u_int32_t tf_t0;
	u_int32_t tf_t1;
	u_int32_t tf_t2;
	u_int32_t tf_t3;
	u_int32_t tf_t4;
	u_int32_t tf_t5;
	u_int32_t tf_t6;
	u_int32_t tf_t7;
	u_int32_t tf_s0;
	u_int32_t tf_s1;
	u_int32_t tf_s2;
	u_int32_t tf_s3;
	u_int32_t tf_s4;
	u_int32_t tf_s5;
	u_int32_t tf_s6;
	u_int32_t tf_s7;
	u_int32_t tf_t8;
	u_int32_t tf_t9;
	u_int32_t tf_k0;	/* dummy (see exception.S comments) */
	u_int32_t tf_k1;	/* dummy */
	u_int32_t tf_gp;
	u_int32_t tf_sp;
	u_int32_t tf_s8;
	u_int32_t tf_epc;	/* coprocessor 0 epc register */
};

/*
 * MIPS exception codes.
 */
#define EX_IRQ    0    /* Interrupt */
#define EX_MOD    1    /* TLB Modify (write to read-only page) */
#define EX_TLBL   2    /* TLB miss on load */
#define EX_TLBS   3    /* TLB miss on store */
#define EX_ADEL   4    /* Address error on load */
#define EX_ADES   5    /* Address error on store */
#define EX_IBE    6    /* Bus error on instruction fetch */
#define EX_DBE    7    /* Bus error on data load *or* store */
#define EX_SYS    8    /* Syscall */
#define EX_BP     9    /* Breakpoint */
#define EX_RI     10   /* Reserved (illegal) instruction */
#define EX_CPU    11   /* Coprocessor unusable */
#define EX_OVF    12   /* Arithmetic overflow */

/*
 * Trapframe-related functions.
 *
 * The trapframe must be on the thread's own stack or bad things will
 * happen.
 */
void mips_usermode(struct trapframe *tf);
void md_forkentry(struct trapframe *tf);

#endif /* _MIPS_TRAPFRAME_H_ */
