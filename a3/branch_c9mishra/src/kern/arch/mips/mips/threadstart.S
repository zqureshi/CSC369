#include <machine/asmdefs.h>
  
   .text
   .set noreorder

   .globl mips_threadstart
   .type mips_threadstart,@function
   .ent mips_threadstart
mips_threadstart:

   /*
    * This code doesn't take normal arguments. It's reached when
    * mips_switch switches to a new thread. mips_switch does "j ra";
    * ra gets preloaded in md_initpcb to come here.
    *
    * Our arguments are in callee-save registers, as follows:
    *
    *   s0   data1
    *   s1   data2
    *   s2   func
    *
    * We need to rearrange these so as to call the normal C function
    * mi_threadstart(void *data1, unsigned long data2, void (*func)(...)).
    */

   addiu sp, sp, -16	/* make our stack frame */
   
   move ra, $0		/* clear return addr so we're top of the call stack */

   move a0, s0		/* load arguments and call */
   move a1, s1
   j mi_threadstart
   move a2, s2		/* (in delay slot) */

   .end mips_threadstart
