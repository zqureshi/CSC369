#ifndef _MIPS_SPECIALREG_H_
#define _MIPS_SPECIALREG_H_

/*
 * Mode bits in c0_status
 */
#define CST_IEc      0x00000001	/* current: interrupt enable */
#define CST_KUc      0x00000002	/* current: user mode */
#define CST_IEp      0x00000004	/* previous: interrupt enable */
#define CST_KUp      0x00000008	/* previous: user mode */
#define CST_IEo      0x00000010	/* old: interrupt enable */
#define CST_KUo      0x00000020	/* old: user mode */
#define CST_MODEMASK 0x0000003f	/* mask for the above */
#define CST_IRQMASK  0x0000ff00	/* mask for the individual irq enable bits */
#define CST_BEV      0x00400000	/* bootstrap exception vectors flag */

/*
 * Fields of the c0_cause register 
 */
#define CCA_UTLB   0x00000001   /* true if UTLB exception (set by our asm) */
#define CCA_CODE   0x0000003c   /* EX_foo in trapframe.h */
#define CCA_IRQS   0x0000ff00   /* Currently pending interrupts */
#define CCA_COPN   0x30000000   /* Coprocessor number for EX_CPU */
#define CCA_JD     0x80000000   /* True if exception happened in jump delay */

#define CCA_CODESHIFT   2       /* shift for CCA_CODE field */

/*
 * Fields of the c0_index register
 */
#define CIN_P      0x80000000   /* nonzero -> TLB probe found nothing */
#define CIN_INDEX  0x00003f00   /* 6-bit index into TLB */

#define CIN_INDEXSHIFT  8       /* shift for CIN_INDEX field */

#endif /* _MIPS_SPECIALREG_H_ */
