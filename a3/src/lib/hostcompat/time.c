/*
 * OS/161 __time implementation in terms of Unix gettimeofday().
 */

#include <sys/types.h>
#include <sys/time.h>
#include <string.h>	/* sometimes required for NULL */

#include "hostcompat.h"

time_t
__time(time_t *secs, unsigned long *nsecs)
{
	struct timeval tv;
	if (gettimeofday(&tv, NULL) < 0) {
		return -1;
	}
	if (secs) {
		*secs = tv.tv_sec;
	}
	if (nsecs) {
		*nsecs = tv.tv_usec * 1000;
	}
	return tv.tv_sec;
}
