#include <unistd.h>

/*
 * POSIX C function: retrieve time in seconds since the epoch.
 * Uses the OS/161 system call __time, which does the same thing
 * but also returns nanoseconds.
 */

time_t
time(time_t *t)
{
	return __time(t, NULL);
}
