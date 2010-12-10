#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <sys/types.h>

/*
 * Ascii to integer - turn a string holding a number into a number.
 */
int atoi(const char *);

/*
 * Standard routine to bail out of a program in a severe error condition.
 */
void abort(void);

/*
 * Routine to exit cleanly.
 * (This does libc cleanup before calling the _exit system call.)
 */
void exit(int code);

/*
 * Run a command. Returns its exit status as it comes back from waitpid().
 */
int system(const char *command);

/*
 * Pseudo-random number generator.
 */
#define RAND_MAX  0x7fffffff
long random(void);
void srandom(unsigned long seed);

/*
 * Memory allocation functions.
 */
void *malloc(size_t size);
void free(void *ptr);

#endif /* _STDLIB_H_ */
