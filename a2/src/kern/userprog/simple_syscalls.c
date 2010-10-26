// BEGIN EX1 SOLUTION
// New file for Exercise 1

#include <types.h>
#include <lib.h>
#include <kern/limits.h>
#include <kern/errno.h>
#include <thread.h>


// Print a user-supplied char value to the console.  Although not
// all values in the range 0-255 are printable characters, none of
// them will do any damage to the console if sent via kprintf, so 
// we just pass them along directly without any error checking.
// As with sys_helloworld, we do return an error if kprintf doesn't
// claim to have sent the char to the console.

int sys_printchar(char c, int *retval) {
	*retval = kprintf("%c",c);
	if (*retval != 1) {
		return EIO;
	}
	return 0;
}

// Just calls thread_exit with the exitcode

int sys_exit(int exitcode) {

	thread_exit(exitcode);

	// thread_exit doesn't return... if it does, something has already
	// gone horribly wrong, but we'll just tell the user-level that
	// they should try again.
	//
	return EAGAIN;
}

// END EX1 SOLUTION
