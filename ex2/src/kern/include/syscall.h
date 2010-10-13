#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */

int sys_reboot(int code);
void sys__exit(int exitcode);
int sys_printchar(char c);

#endif /* _SYSCALL_H_ */
