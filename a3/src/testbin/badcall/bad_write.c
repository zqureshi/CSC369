/*
 * Invalid calls to write()
 */

#include "test.h"

void
test_write(void)
{
	test_write_fd();
	test_write_buf();
}
