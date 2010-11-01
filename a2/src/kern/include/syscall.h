#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */

int sys_reboot(int code);

// BEGIN EX2 SOLUTION
int sys_printchar(char c, int *retval);
int sys_exit(int exitcode);
// END EX2 SOLUTION

// BEGIN A2 SETUP
int sys_read(int fd, userptr_t buf, size_t size, int *retval);
int sys_write(int fd, userptr_t buf, size_t size, int *retval);
// END A2 SETUP
pid_t sys_getpid();
int sys_execv(char *progname, char **args);


#endif /* _SYSCALL_H_ */
