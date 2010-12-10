#include <machine/asmdefs.h>

   /*
    * The actual mechanism for flushing the caches on MIPS is nasty.
    * Here, we take advantage of the fact that we know we are running
    * on the System/161 simulator, which doesn't have caches, and
    * treat cache flushing as a no-op.
    *
    * Should you ever attempt to run on a real MIPS, you will need to
    * implement these functions properly.
    */

   .text
   .set noreorder

   .globl mips_flushicache
   .type mips_flushicache,@function
   .ent mips_flushicache
mips_flushicache:
   j ra
   nop
   .end mips_flushicache

