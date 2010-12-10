#include <unistd.h>

/*
 * sync - force kernel buffers (write-back disk cache) to disk.
 *
 * Just calls the sync() system call.
 *
 * Takes the usual argc/argv arguments, but does not use them.
 */

int
cmd_sync(int argc, char *argv[])
{
        (void)argc;
	(void)argv;

	sync();
	return 0;
}
