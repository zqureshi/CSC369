#include <string.h>

/*
 * Standard C string function: compare two memory blocks and return
 * their sort order.
 */

int
memcmp(const void *av, const void *bv, size_t len)
{
	const unsigned char *a = av;
	const unsigned char *b = bv;
	size_t i;

	for (i=0; i<len; i++) {
		if (a[i] != b[i]) {
			return (int)(a[i] - b[i]);
		}
	}
	return 0;
}
