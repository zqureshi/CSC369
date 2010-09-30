#ifndef _KERN_ERRMSG_H_
#define _KERN_ERRMSG_H_

/*
 * Error strings.
 * This table must agree with kern/errno.h.
 *
 * Note that since this actually defines sys_errlist and sys_nerrlist, it
 * should only be included in one file. For the kernel, that file is 
 * lib/misc.c; for userland it's lib/libc/strerror.c.
 */
const char *const sys_errlist[] = {
	"Operation succeeded",        /* 0 */
	"Invalid argument",           /* EINVAL */
	"Bad memory address",         /* EFAULT */
	"String too long",            /* ENAMETOOLONG */
	"Out of memory",              /* ENOMEM */
	"Input/output error",         /* EIO */
	"No such file or directory",  /* ENOENT */
	"Not a directory",            /* ENOTDIR */
	"Is a directory",             /* EISDIR */
	"File or object exists",      /* EEXIST */
	"Cross-device link",          /* EXDEV */
	"Try again later",            /* EAGAIN */
	"Illegal seek",               /* ESPIPE */
	"Unimplemented feature",      /* EUNIMP */
	"Device not available",       /* ENXIO */
	"No such device",             /* ENODEV */
	"Device or resource busy",    /* EBUSY */
	"Invalid/inappropriate ioctl",/* EIOCTL (ENOTTY in Unix) */
	"Directory not empty",        /* ENOTEMPTY */
	"Result too large",           /* ERANGE */
	"No space left on device",    /* ENOSPC */
	"Too many open files",        /* EMFILE */
	"Too many open files in system",/* ENFILE */
	"No such system call",        /* ENOSYS */
	"File is not executable",     /* ENOEXEC */
	"Argument list too long",     /* E2BIG */
	"Bad file number",            /* EBADF */
};

/*
 * Number of entries in sys_errlist.
 */
const int sys_nerr = sizeof(sys_errlist)/sizeof(const char *);

#endif /* _KERN_ERRMSG_H_ */
