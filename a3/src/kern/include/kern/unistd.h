#ifndef _KERN_UNISTD_H_
#define _KERN_UNISTD_H_

/*
 * Constants for system calls
 */

/* Flags for open: choose one of these: */
#define O_RDONLY      0      /* Open for read */
#define O_WRONLY      1      /* Open for write */
#define O_RDWR        2      /* Open for read and write */
/* then or in any of these: */
#define O_CREAT       4      /* Create file if it doesn't exist */
#define O_EXCL        8      /* With O_CREAT, fail if file already exists */
#define O_TRUNC      16      /* Truncate file upon open */
#define O_APPEND     32      /* All writes happen at EOF (optional feature) */

/* Additional related definition */
#define O_ACCMODE     3      /* mask for O_RDONLY/O_WRONLY/O_RDWR */


/* Constants for read/write/etc: special file handles */
#define STDIN_FILENO  0      /* Standard input */
#define STDOUT_FILENO 1      /* Standard output */
#define STDERR_FILENO 2      /* Standard error */

/* Codes for reboot */
#define RB_REBOOT     0      /* Reboot system */
#define RB_HALT       1      /* Halt system and do not reboot */
#define RB_POWEROFF   2      /* Halt system and power off */

/* Codes for lseek */
#define SEEK_SET      0      /* Seek relative to beginning of file */
#define SEEK_CUR      1      /* Seek relative to current position in file */
#define SEEK_END      2      /* Seek relative to end of file */

/* The codes for ioctl are in kern/ioctl.h */
/* The codes for stat/fstat/lstat are in kern/stat.h */

#endif /* _KERN_UNISTD_H_ */
