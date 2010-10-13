/*
 * CSC 369 Fall 2010 - Excercise 2
 *
 * Zeeshan Qureshi
 * g0zee@cdf.toronto.edu
 * $td$
 */

#include<types.h>
#include<lib.h>
#include<thread.h>

/* Handler for SYS__exit */
void sys__exit(int exitcode){
  /* Call thread_exit with exitcode to cleanup */
  thread_exit(exitcode);
}

/* Handler for SYS_printchar */
int sys_printchar(char c){
  /* Check return val of kprintf to see if error occurred */
  if(kprintf("%c", c) == 1)
    return 1;    /* Success */
  else
    return -1;   /* Failure */
}
