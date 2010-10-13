/* Automatically generated from syscalls-mips.S */
/*
 * This file is copied to syscalls.S, and then the actual syscalls are
 * appended as lines of the form
 *    SYSCALL(symbol, number)
 *
 * Warning: gccs before 3.0 run cpp in -traditional mode on .S files.
 * So if you use an older gcc you'll need to change the token pasting
 * in SYSCALL().
 */

#include <kern/callno.h>
#include <machine/asmdefs.h>

/*
 * Definition for each syscall.
 * All we do is load the syscall number into v0, the register the
 * kernel expects to find it in, and jump to the shared syscall code.
 * (Note that the addiu instruction is in the jump's delay slot.)
 */    
#define SYSCALL(sym, num) \
   .set noreorder		; \
   .globl sym			; \
   .type sym,@function		; \
   .ent sym			; \
sym:				; \
   j __syscall                  ; \
   addiu v0, $0, SYS_##sym     	; \
   .end sym			; \
   .set reorder

/*
 * Now, the shared system call code.
 * The MIPS syscall ABI is as follows:	
 *
 *    On entry, call number in v0. The rest is like a normal function
 *    call: four args in a0-a3, the other args on the stack.
 *
 *    On successful return, zero in a3 register; return value in v0
 *    (v0 and v1 for a 64-bit return value).
 *
 *    On error return, nonzero in a3 register; errno value in v0.
 *
 * The use of a3 as a return register to hold the success flag is
 * gross, but I didn't make it up.
 *
 * Note that by longstanding Unix convention and POSIX decree, errno
 * is not to be set unless the call actually fails.
 */

   .set noreorder
   .text
   .type __syscall,@function
   .ent __syscall
__syscall:
   syscall              /* make system call */
   beq a3, $0, 1f       /* if a3 is zero, call succeeded */
   nop			/* delay slot */
   sw v0, errno        /* call failed: store errno */
   li v1, -1		/* and force return value to -1 */
   li v0, -1
1:
   j ra			/* return */
   nop			/* delay slot */
   .end __syscall
   .set reorder

SYSCALL(_exit, 0)
SYSCALL(execv, 1)
SYSCALL(fork, 2)
SYSCALL(waitpid, 3)
SYSCALL(open, 4)
SYSCALL(read, 5)
SYSCALL(write, 6)
SYSCALL(close, 7)
SYSCALL(reboot, 8)
SYSCALL(sync, 9)
SYSCALL(sbrk, 10)
SYSCALL(getpid, 11)
SYSCALL(ioctl, 12)
SYSCALL(lseek, 13)
SYSCALL(fsync, 14)
SYSCALL(ftruncate, 15)
SYSCALL(fstat, 16)
SYSCALL(remove, 17)
SYSCALL(rename, 18)
SYSCALL(link, 19)
SYSCALL(mkdir, 20)
SYSCALL(rmdir, 21)
SYSCALL(chdir, 22)
SYSCALL(getdirentry, 23)
SYSCALL(symlink, 24)
SYSCALL(readlink, 25)
SYSCALL(dup2, 26)
SYSCALL(pipe, 27)
SYSCALL(__time, 28)
SYSCALL(__getcwd, 29)
SYSCALL(stat, 30)
SYSCALL(lstat, 31)
SYSCALL(printchar, 40)
