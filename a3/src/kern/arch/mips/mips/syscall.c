#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <machine/pcb.h>
#include <machine/spl.h>
#include <machine/trapframe.h>
#include <kern/callno.h>
#include <syscall.h>
#include <thread.h>

/*
 * System call handler.
 *
 * A pointer to the trapframe created during exception entry (in
 * exception.S) is passed in.
 *
 * The calling conventions for syscalls are as follows: Like ordinary
 * function calls, the first 4 32-bit arguments are passed in the 4
 * argument registers a0-a3. In addition, the system call number is
 * passed in the v0 register.
 *
 * On successful return, the return value is passed back in the v0
 * register, like an ordinary function call, and the a3 register is
 * also set to 0 to indicate success.
 *
 * On an error return, the error code is passed back in the v0
 * register, and the a3 register is set to 1 to indicate failure.
 * (Userlevel code takes care of storing the error code in errno and
 * returning the value -1 from the actual userlevel syscall function.
 * See src/lib/libc/syscalls.S and related files.)
 *
 * Upon syscall return the program counter stored in the trapframe
 * must be incremented by one instruction; otherwise the exception
 * return code will restart the "syscall" instruction and the system
 * call will repeat forever.
 *
 * Since none of the OS/161 system calls have more than 4 arguments,
 * there should be no need to fetch additional arguments from the
 * user-level stack.
 *
 * Watch out: if you make system calls that have 64-bit quantities as
 * arguments, they will get passed in pairs of registers, and not
 * necessarily in the way you expect. We recommend you don't do it.
 * (In fact, we recommend you don't use 64-bit quantities at all. See
 * arch/mips/include/types.h.)
 */

void
mips_syscall(struct trapframe *tf)
{
	int callno;
	int32_t retval;
	int err;

	assert(curspl==0);

	callno = tf->tf_v0;

	/*
	 * Initialize retval to 0. Many of the system calls don't
	 * really return a value, just 0 for success and -1 on
	 * error. Since retval is the value returned on success,
	 * initialize it to 0 by default; thus it's not necessary to
	 * deal with it except for calls that return other values, 
	 * like write.
	 */

	retval = 0;

	switch (callno) {
	    case SYS_reboot:
		err = sys_reboot(tf->tf_a0);
		break;

	    /* Add stuff here */

	    // BEGIN A0 SOLUTION
	    
	    // Note the pattern - each system call is handled by a function
	    // named "sys_<user level system function name>", so a user-level
	    // call to "helloworld" is handled by calling the system 
	    // function "sys_helloworld".  This is just a convention, but a
	    // useful one to improve code readability.  
	    //
            // Also note the convention for passing arguments:  they all
	    // come from the registers saved in the trapframe, but anything
            // that represents a pointer to a user address is cast to
            // "userptr_t" before passing it to the handler function.  This
	    // makes it explicit that we cannot use the address as an ordinary
	    // pointer. For helloworld and _exit, there are no examples of 
	    // this.
            //
            // Finally, note that each function returns an error code (0 if
	    // no error occurs) and is passed a pointer to "retval", so
	    // that the actual non-error return value of the system call can
            // be set within the handler function itself.

	    case SYS_helloworld:
		err = sys_helloworld(&retval);
		break;

	    case SYS__exit:
		err = sys__exit(tf->tf_a0);
		break;

	    // END A0 SOLUTION

	    // BEGIN ASST1 SOLUTION
            case SYS_fork:
                err = sys_fork(tf, &retval);
                break;

            case SYS_getpid:
                err = sys_getpid(&retval);
                break;

            case SYS_waitpid:
                err = sys_waitpid(tf->tf_a0, (userptr_t)tf->tf_a1, tf->tf_a2,
                                  &retval);
                break;
	    // END ASST1 SOLUTION

	    // BEGIN A3 SETUP
            
	    case SYS_open:
		err = sys_open((userptr_t)tf->tf_a0, tf->tf_a1, tf->tf_a2, 
			       &retval);
		break;
	    case SYS_close:
		err = sys_close(tf->tf_a0);
		break;
	    case SYS_read:
		err = sys_read(tf->tf_a0, (userptr_t)tf->tf_a1, tf->tf_a2, 
			       &retval);
		break;
	    case SYS_write:
		err = sys_write(tf->tf_a0, (userptr_t)tf->tf_a1, tf->tf_a2, 
				&retval);
		break;
	    case SYS_lseek:
		err = sys_lseek(tf->tf_a0, tf->tf_a1, tf->tf_a2, &retval);
		break;
	    case SYS_dup2:
		err = sys_dup2(tf->tf_a0, tf->tf_a1, &retval);
		break;
	    case SYS_chdir:
		err = sys_chdir((userptr_t)tf->tf_a0);
		break;
	    case SYS___getcwd:
		err = sys___getcwd((userptr_t)tf->tf_a0, tf->tf_a1, &retval);
		break;
	    case SYS_remove:
		err = sys_remove((userptr_t)tf->tf_a0);
		break;
	    case SYS_rename:
		err = sys_rename((userptr_t)tf->tf_a0, (userptr_t)tf->tf_a1);
		break;
	    case SYS_getdirentry:
		err = sys_getdirentry(tf->tf_a0, (userptr_t)tf->tf_a1, 
				      tf->tf_a2, &retval);
		break;
	    case SYS_fstat:
		err = sys_fstat(tf->tf_a0, (userptr_t)tf->tf_a1);
		break;

	    case SYS_mkdir:
		err = sys_mkdir((userptr_t)tf->tf_a0, tf->tf_a1);
		break;
	    case SYS_rmdir:
		err = sys_rmdir((userptr_t)tf->tf_a0);
		break;
	    
	    // END A3 SETUP

	    default:
		kprintf("Unknown syscall %d\n", callno);
		err = ENOSYS;
		break;
	}


	if (err) {
		/*
		 * Return the error code. This gets converted at
		 * userlevel to a return value of -1 and the error
		 * code in errno.
		 */
		tf->tf_v0 = err;
		tf->tf_a3 = 1;      /* signal an error */
	}
	else {
		/* Success. */
		tf->tf_v0 = retval;
		tf->tf_a3 = 0;      /* signal no error */
	}
	
	/*
	 * Now, advance the program counter, to avoid restarting
	 * the syscall over and over again.
	 */
	
	tf->tf_epc += 4;

	/* Make sure the syscall code didn't forget to lower spl */
	assert(curspl==0);
}


void
md_forkentry(void *tf, unsigned long junk)
{
	/*
	 * Complete replacement of original md_forkentry
	 */
	struct trapframe mytf;
	(void)junk;

	/*
	 * Now copy the trapframe to our stack, so we can free the one
	 * that was malloc'd and use the one on our stack for going to
	 * userspace.
	 */

	mytf = *(struct trapframe *)tf;
	kfree(tf);

	/*
         * Succeed and return 0.
         */
        mytf.tf_v0 = 0;
        mytf.tf_a3 = 0;

        /*
         * Advance the PC.
         */
        mytf.tf_epc += 4;

        mips_usermode(&mytf);

}
