/*
 * Process-related syscalls.
 * New for ASST1.
 */

#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <thread.h>
#include <curthread.h>
#include <pid.h>
#include <clock.h>
#include <syscall.h>
#include <machine/trapframe.h>


/*
 * sys_getpid
 * love easy syscalls. :)
 */
int
sys_getpid(pid_t *retval)
{
	*retval = curthread->t_pid;
	return 0;
}


/*
 * sys_fork
 * 
 * create a new process, which begins executing in md_forkentry().
 */


int
sys_fork(struct trapframe *tf, pid_t *retval)
{
	struct trapframe *ntf; /* new trapframe, copy of tf */
	int result;

	/*
	 * Copy the trapframe to the heap, because we might return to
	 * userlevel and make another syscall (changing the trapframe)
	 * before the child runs. The child will free the copy.
	 */

	ntf = kmalloc(sizeof(struct trapframe));
	if (ntf==NULL) {
		return ENOMEM;
	}
	*ntf = *tf; /* copy the trapframe */

	result = thread_fork(curthread->t_name, ntf, 0,
			     md_forkentry, retval);
	if (result) {
		kfree(ntf);
		return result;
	}

	return 0;
}

/*
 * sys_waitpid
 * just pass off the work to the thread_join code.
 * In our implementation, the flags are ignored.
 */
int
sys_waitpid(pid_t pid, userptr_t retstatus, int flags, pid_t *retval)
{
	int status; 
	int result;
	(void)flags;

	result = thread_join(pid, &status);
	if (result) {
		return result;
	}

	*retval = pid;

	return copyout(&status, retstatus, sizeof(int));
}
