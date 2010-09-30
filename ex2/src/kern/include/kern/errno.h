#ifndef _KERN_ERRNO_H_
#define _KERN_ERRNO_H_

/*
 * If you change this, be sure to make appropriate corresponding changes
 * to kern/errmsg.h as well. You might also want to change the man page
 * for errno to document the new error.
 *
 * Also note that this file has to work from assembler, so it should
 * contain only symbolic constants.
 */

#define EINVAL       1      /* Invalid argument */
#define EFAULT       2      /* Bad memory reference */
#define ENAMETOOLONG 3      /* String too long */
#define ENOMEM       4      /* Out of memory */
#define EIO          5      /* Input/output error */
#define ENOENT       6      /* No such file or directory */
#define ENOTDIR      7      /* Not a directory */
#define EISDIR       8      /* Is a directory */
#define EEXIST       9      /* File exists */
#define EXDEV        10     /* Cross-device link */
#define EAGAIN       11     /* Try again later */
#define ESPIPE       12     /* Illegal seek */
#define EUNIMP       13     /* Unimplemented feature */
#define ENXIO        14     /* Device not available */
#define ENODEV       15     /* No such device */
#define EBUSY        16     /* Device busy */
#define EIOCTL       17     /* Invalid or inappropriate ioctl */
#define ENOTEMPTY    18     /* Directory not empty */
#define ERANGE       19     /* Result too large */
#define ENOSPC       20     /* No space left on device */
#define EMFILE       21     /* Too many open files */
#define ENFILE       22     /* Too many open files in system */
#define ENOSYS       23     /* No such system call */
#define ENOEXEC      24     /* File is not executable */
#define E2BIG        25     /* Argument list too long */
#define EBADF        26     /* Bad file number */

#endif /* _KERN_ERRNO_H_ */
