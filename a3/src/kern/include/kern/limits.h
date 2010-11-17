#ifndef _KERN_LIMITS_H_
#define _KERN_LIMITS_H_

/* Longest filename (without directory) not including null terminator */
#define NAME_MAX  255

/* Longest full path name */
#define PATH_MAX   1024

// BEGIN ASST1
/* min value for a process ID (that can be assigned to a user process) */
#define PID_MIN	2

/* max value for a process ID */
#define PID_MAX	32767

/* max number of processes at once */
#define PROCS_MAX	128

/* max bytes for an exec function */
#define ARG_MAX	32768

/* max number of arguments for an exec function */
#define NARG_MAX	1024

// END ASST1

// BEGIN ASST3 SETUP

/* Maximum number of open file descriptors per process */
#define FOPEN_MAX  16

// END ASST3 SETUP

#endif /* _KERN_LIMITS_H_ */
