#include <string.h>

/*
 * C standard function - initialize a block of memory
 */

void *
memset(void *ptr, int ch, size_t len)
{
	char *p = ptr;
	size_t i;
	
	for (i=0; i<len; i++) {
		p[i] = ch;
	}

	return ptr;
}
