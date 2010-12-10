#include <machine/asmdefs.h>
#include <machine/specialreg.h>

   .text
   .set noreorder

   /*
    * TLB_Random: use the "tlbwr" instruction to write a TLB entry
    * into a (very pseudo-) random slot in the TLB.
    */
   .globl TLB_Random
   .type TLB_Random,@function
   .ent TLB_Random
TLB_Random:
   mtc0 a0, c0_entryhi	/* store the passed entry into the */
   mtc0 a1, c0_entrylo	/*   tlb entry registers */
   tlbwr		/* do it */
   j ra
   nop
   .end TLB_Random

   /*
    * TLB_Write: use the "tlbwi" instruction to write a TLB entry
    * into a selected slot in the TLB.
    */
   .text   
   .globl TLB_Write
   .type TLB_Write,@function
   .ent TLB_Write
TLB_Write:
   mtc0 a0, c0_entryhi	/* store the passed entry into the */
   mtc0 a1, c0_entrylo	/*   tlb entry registers */
   sll  t0, a2, CIN_INDEXSHIFT  /* shift the passed index into place */
   mtc0 t0, c0_index	/* store the shifted index into the index register */
   tlbwi		/* do it */
   j ra
   nop
   .end TLB_Write

   /*
    * TLB_Read: use the "tlbr" instruction to read a TLB entry
    * from a selected slot in the TLB.
    */
   .text
   .globl TLB_Read
   .type TLB_Read,@function
   .ent TLB_Read
TLB_Read:
   sll  t0, a2, CIN_INDEXSHIFT  /* shift the passed index into place */
   mtc0 t0, c0_index	/* store the shifted index into the index register */
   tlbr			/* do it */
   mfc0 t0, c0_entryhi	/* get the tlb entry out of the */
   mfc0 t1, c0_entrylo	/*   tlb entry registers */
   sw t0, 0(a0)		/* store through the */
   sw t1, 0(a1)		/*   passed pointers */
   j ra
   nop
   .end TLB_Read

   /*
    * TLB_Probe: use the "tlbp" instruction to find the index in the
    * TLB of a TLB entry matching the relevant parts of the one supplied.
    */
   .text
   .globl TLB_Probe
   .type TLB_Probe,@function
   .ent TLB_Probe
TLB_Probe:
   mtc0 a0, c0_entryhi	/* store the passed entry into the */
   mtc0 a1, c0_entrylo	/*   tlb entry registers */
   tlbp			/* do it */
   mfc0 t0, c0_index	/* fetch the index back in t0 */

   /*
    * If the high bit (CIN_P) of c0_index is set, the probe failed.
    * The high bit is not set <--> c0_index (now in t0) >= 0.
    */

   bgez t0, 1f		/* did probe succeed? if so, skip forward */
   nop			/* delay slot */
   addi v0, z0, -1	/* set return value to -1 to indicate failure */
   j ra			/* done */
   nop			/* delay slot */

1:
   /* succeeded - get the index field from the index register value */
   andi t1, t0, CIN_INDEX       /* mask off the field */
   j ra				/* done */
   sra  v0, t1, CIN_INDEXSHIFT  /* shift it (in delay slot) */
   .end TLB_Probe


   /*
    * TLB_Reset
    *
    * Initialize the TLB. At processor startup, the TLB state is completely
    * undefined. So be sure to avoid creating any duplicates. Also make sure
    * that the initialization entries don't duplicate the INVALID entries
    * defined in tlb.h. (This way you can write the invalid entries in
    * without having to use tlbp to find out if they're going to cause dups.)
    *
    * This function is not defined in tlb.h because it's only called from
    * start.S.
    */
   .text
   .globl TLB_Reset
   .type TLB_Reset,@function
   .ent TLB_Reset
TLB_Reset:
   li t0, 0			/* t0 <- tlb index number (shifted) */
   li t1, 0x81000000		/* t1 <- tlb reset vaddr */
1:
   mtc0 $0, c0_entrylo 		/* set up proposed tlb entry for reset */
   mtc0 t1, c0_entryhi
   tlbp				/* check if it already exists */
   mfc0 t2, c0_index
   bgez t2, 1b			/* if it does, loop back */
   addiu t1, t1, 0x1000		/* next vaddr (in delay slot) */
   mtc0 t0, c0_index		/* doesn't exist, set index to write to */
   addiu t0, t0, 0x100		/* next tlb index (shifted) */
   bne t0, 0x4000, 1b		/* if it's not the last tlb index, loop */
   tlbwi			/* write tlb entry (in delay slot) */
   j ra				/* done */
   nop				/* delay slot */	
   .end TLB_Reset
