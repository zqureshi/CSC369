#ifndef _KERN_CALLNO_H_
#define _KERN_CALLNO_H_

/*
 * System call numbers.
 * Caution: this file is parsed by a shell script to generate the assembly
 * language system call stubs. Don't add weird stuff between the markers.
 */

/*CALLBEGIN*/
#define SYS__exit        0
#define SYS_execv        1
#define SYS_fork         2
#define SYS_waitpid      3
#define SYS_open         4
#define SYS_read         5
#define SYS_write        6
#define SYS_close        7
#define SYS_reboot       8
#define SYS_sync         9
#define SYS_sbrk         10
#define SYS_getpid       11
#define SYS_ioctl        12
#define SYS_lseek        13
#define SYS_fsync        14
#define SYS_ftruncate    15
#define SYS_fstat        16
#define SYS_remove       17
#define SYS_rename       18
#define SYS_link         19
#define SYS_mkdir        20
#define SYS_rmdir        21
#define SYS_chdir        22
#define SYS_getdirentry  23
#define SYS_symlink      24
#define SYS_readlink     25
#define SYS_dup2         26
#define SYS_pipe         27
#define SYS___time       28
#define SYS___getcwd     29
#define SYS_stat         30
#define SYS_lstat        31
/*CALLEND*/


#endif /* _KERN_CALLNO_H_ */
