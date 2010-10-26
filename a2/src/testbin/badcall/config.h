/*
 * Note: if the assumptions in the constants below are violated by
 * your system design, please change the values as necessary. Don't
 * change stuff in the .c files, or disable tests, without consulting
 * the course staff first.
 */

#if defined(__mips__)
#define KERN_PTR	((void *)0x80000000)	/* addr within kernel */
#define INVAL_PTR	((void *)0x40000000)	/* addr not part of program */
#else
#error "Please fix this"
#endif

/*
 * We assume CLOSED_FD is a legal fd that won't be open when we're running.
 * CLOSED_FD+1 should also be legal and not open.
 */
#define CLOSED_FD		10

/* We assume IMPOSSIBLE_FD is a fd that is completely not allowed. */
#define IMPOSSIBLE_FD		1234567890

/* We assume this pid won't exist while we're running. Change as needed. */
#define NONEXIST_PID		34000

/* The symbolic error code for no such process (ESRCH in unix) */
#ifdef ESRCH
#define NOSUCHPID_ERROR		ESRCH
#else
#define NOSUCHPID_ERROR		EINVAL
#endif

/* An arbitrary process exit code that hopefully won't occur by accident */
#define MAGIC_STATUS		107

/* An ioctl that doesn't exist */
#define NONEXIST_IOCTL		12345
