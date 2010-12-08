#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */

int sys_reboot(int code);

// BEGIN A0 SOLUTION
int sys_helloworld(int *retval);
int sys_printchar(char c);
int sys__exit(int exitcode);

// END A0 SOLUTION 

// ASST1 SOLUTION
int sys_fork(struct trapframe *tf, pid_t *retval);
int sys_waitpid(pid_t pid, userptr_t returncode, int flags, pid_t *retval);
int sys_getpid(pid_t *retval);

// BEGIN A3 SETUP
int sys_open(userptr_t filename, int flags, int mode, int *retval);
int sys_read(int fd, userptr_t buf, size_t size, int *retval);
int sys_write(int fd, userptr_t buf, size_t size, int *retval);
int sys_close(int fd);
int sys_lseek(int fd, off_t offset, int code, off_t *retval);
int sys_dup2(int oldfd, int newfd, int *retval);
int sys_chdir(userptr_t path);
int sys___getcwd(userptr_t buf, size_t buflen, int *retval);
int sys_remove(userptr_t path);
int sys_rename(userptr_t oldpath, userptr_t newpath);
int sys_getdirentry(int fd, userptr_t buf, size_t buflen, int *retval);
int sys_fstat(int fd, userptr_t statptr);
int sys_mkdir(userptr_t path, int mode);
int sys_rmdir(userptr_t path);

// END A3 SETUP

#endif /* _SYSCALL_H_ */
