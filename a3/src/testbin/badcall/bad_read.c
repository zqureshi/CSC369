/*
 * Invalid calls to read()
 */

#include "test.h"

void
test_read(void)
{
	test_read_fd();
	test_read_buf();
}

