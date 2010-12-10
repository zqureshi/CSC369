#ifndef _SETJMP_H_
#define _SETJMP_H_

/* Get the (machine-dependent) definition of jmp_buf. */
#include <machine/setjmp.h>

/*
 * Functions.
 *
 * setjmp saves the current processor state in the jmp_buf and 
 * returns 0. A subsequent call to longjmp with the same jmp_buf
 * causes execution to return to where setjmp was called. setjmp
 * returns a second time, this time returning CODE. (If CODE is
 * 0, it is forced to 1.)
 *
 * If the stack frame that called setjmp returns before longjmp is
 * called, the results are undefined. 
 */

int setjmp(jmp_buf jb);
void longjmp(jmp_buf jb, int code);

#endif /* _SETJMP_H_ */
