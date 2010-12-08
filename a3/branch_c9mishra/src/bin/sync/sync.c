#include <unistd.h>

/*
 * sync - force kernel buffers (write-back disk cache) to disk.
 *
 * Just calls the sync() system call.
 */

int
main()
{
	sync();
	return 0;
}
