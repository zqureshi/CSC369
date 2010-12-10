#include <machine/asmdefs.h>
  
   .text
   .set noreorder
  
   .globl mips_switch
   .type mips_switch,@function
   .ent mips_switch
mips_switch:
   /*
    * a0 contains a pointer to the old thread's struct pcb.
    * a1 contains a pointer to the new thread's struct pcb.
    *
    * The only thing we touch in the pcb is the first word, which
    * we save the stack pointer in. The other registers get saved
    * on the stack, namely:
    *
    *      s0-s8
    *      gp, ra
    *
    * The order must match arch/mips/include/switchframe.h.
    */

   /* Allocate stack space for saving 11 registers. 11*4 = 44 */
   addi sp, sp, -44

   /* Save the registers */
   sw	ra, 40(sp)
   sw	gp, 36(sp)
   sw	s8, 32(sp)
   sw	s7, 28(sp)
   sw	s6, 24(sp)
   sw	s5, 20(sp)
   sw	s4, 16(sp)
   sw	s3, 12(sp)
   sw	s2, 8(sp)
   sw	s1, 4(sp)
   sw	s0, 0(sp)

   /* Store the old stack pointer in the old pcb */
   sw	sp, 0(a0)

   /* Get the new stack pointer from the new pcb */
   lw	sp, 0(a1)
   nop		 /* delay slot for load */

   /* Now, restore the registers */
   lw	s0, 0(sp)
   lw	s1, 4(sp)
   lw	s2, 8(sp)
   lw	s3, 12(sp)
   lw	s4, 16(sp)
   lw	s5, 20(sp)
   lw	s6, 24(sp)
   lw	s7, 28(sp)
   lw	s8, 32(sp)
   lw	gp, 36(sp)
   lw	ra, 40(sp)
   nop			/* delay slot for load */

   /* and return. */
   j ra
   addi	sp, sp, 44	/* in delay slot */
   .end mips_switch
