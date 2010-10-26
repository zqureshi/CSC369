/*
 * Macros for register numbers for MIPS r2k/r3k.
 */
#ifndef _MIPS_ASMDEFS_H_
#define _MIPS_ASMDEFS_H_

/*
 * General registers
 */
#define z0  $0     /* always zero register */
#define AT  $1     /* assembler temp register */
#define v0  $2     /* value 0 */
#define v1  $3     /* value 1 */
#define a0  $4     /* argument 0 */
#define a1  $5     /* argument 1 */
#define a2  $6     /* argument 2 */
#define a3  $7     /* argument 3 */
#define t0  $8     /* temporary (caller-save) 0 */
#define t1  $9     /* temporary (caller-save) 1 */
#define t2  $10    /* temporary (caller-save) 2 */
#define t3  $11    /* temporary (caller-save) 3 */
#define t4  $12    /* temporary (caller-save) 4 */
#define t5  $13    /* temporary (caller-save) 5 */
#define t6  $14    /* temporary (caller-save) 6 */
#define t7  $15    /* temporary (caller-save) 7 */
#define s0  $16    /* saved (callee-save) 0 */
#define s1  $17    /* saved (callee-save) 1 */
#define s2  $18    /* saved (callee-save) 2 */
#define s3  $19    /* saved (callee-save) 3 */
#define s4  $20    /* saved (callee-save) 4 */
#define s5  $21    /* saved (callee-save) 5 */
#define s6  $22    /* saved (callee-save) 6 */
#define s7  $23    /* saved (callee-save) 7 */
#define t8  $24    /* temporary (caller-save) 8 */
#define t9  $25    /* temporary (caller-save) 9 */
#define k0  $26    /* kernel temporary 0 */
#define k1  $27    /* kernel temporary 1 */
#define gp  $28    /* global pointer */
#define sp  $29    /* stack pointer */
#define s8  $30    /* saved 8 = frame pointer */
#define ra  $31    /* return address */

/* Coprocessor 0 (system processor) registers */
#define c0_index    $0		/* TLB entry index register */
#define c0_random   $1		/* TLB random slot register */
#define c0_entrylo  $2		/* TLB entry contents (low-order half) */
/*      c0_entrylo0 $2 */	/* MIPS r4k+ only */
/*      c0_entrylo1 $3 */	/* MIPS r4k+ only */
#define c0_context  $4		/* some precomputed pagetable stuff (ignore) */
/*	c0_pagemask $5 */	/* MIPS r4k+ only */
/*	c0_wired    $6 */	/* MIPS r4k+ only */
#define c0_vaddr    $8		/* virtual addr of failing memory access */
/*	c0_count    $9 */	/* MIPS r4k+ only */
#define c0_entryhi  $10		/* TLB entry contents (high-order half) */
/*	c0_compare  $11 */	/* MIPS r4k+ only */
#define c0_status   $12		/* processor status register */
#define c0_cause    $13		/* exception cause register */
#define c0_epc      $14		/* exception PC register */
#define c0_prid     $15		/* processor ID register */
/*	c0_config   $16 */	/* MIPS r4k+ only */
/*	c0_lladdr   $17 */	/* MIPS r4k+ only */
/*	c0_watchlo  $18 */	/* MIPS r4k+ only */
/*	c0_watchhi  $19 */	/* MIPS r4k+ only */

#endif /* _MIPS_ASMDEFS_H_ */
