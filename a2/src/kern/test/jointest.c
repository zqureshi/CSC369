/*
 * Thread join / detach test code.
 */

#include <types.h>
#include <lib.h>
#include <synch.h>
#include <thread.h>
#include <test.h>
#include <clock.h>

#define NTHREADS      26

/* Function for test threads - each prints the number provided
 * by the parent, then prints a character 10 times, then exits 
 * with the same number the parent passed originally as exit status.
 * Child threads are not synchronized with each other, so these
 * prints will all be interleaved.
 */

static
void
jointestthread(void *junk, unsigned long num)
{
	int i;
	(void)junk;

	/*
	 * These will print interleaved with other threads
	 */
	kprintf("Thread %2lu: ", num);
	for (i=0; i< 10; i++) {
		kprintf("%c", (int)num+64);
	}
	kprintf("\n");
	thread_exit(num);
}

/* Parent creates NTHREADS children, and then waits for each to 
 * exit using thread_join.  The function executed by the children
 * causes each child to exit with the number the parent provided,
 * so we can check whether the correct exit status was returned
 */

int
jointest1(int nargs, char **args)
{
	int i, result;
	pid_t kids[NTHREADS];
	int kid_result;
	int badcount = 0;

	(void)nargs;
	(void)args;

	kprintf("Starting join test 1...\n");

	for (i=0; i<NTHREADS; i++) {
		result = thread_fork("jointest1", NULL, i, jointestthread, NULL);//&kids[i]);
		if (result) {
			panic("jointest1: thread_fork failed: %s\n", 
			      strerror(result));
		}
	}

	for (i=0; i<NTHREADS; i++) {
	  result = thread_join(kids[i], &kid_result);
	  if (result) {
	    kprintf("thread_join on kid %u failed: %s\n", kids[i],strerror(result));
	    badcount++;
	  } else {
	    kprintf("kid %u exited with %d\n", kids[i],kid_result);
	    if (kid_result == i) {
	      kprintf("\t SUCCESS!\n");
	    } else {
	      kprintf("\t FAILED: should have been %d\n",i);
	      badcount++;
	    }
	  }
	}

	kprintf("Join test 1 done - %d failures\n",badcount);
	return 0;
}

// This time, we detach half the threads and try to join all of them.
// The attempts to join with the detached threads should return an
// error code as the result.
int jointest2(int nargs, char **args)
{
	int i, result;
	pid_t kids[NTHREADS];
	int kid_result;
	int badcount = 0;

	(void)nargs;
	(void)args;

	kprintf("Starting join test 2...\n");

	for (i=0; i<NTHREADS; i++) {
		result = thread_fork("jointest2", NULL, i, jointestthread, NULL);//&kids[i]);
		if (result) {
			panic("jointest1: thread_fork failed: %s\n", 
			      strerror(result));
		}
		if (i%2) {
		  result = thread_detach(kids[i]);
		  if (result) {
		    kprintf("thread_detach FAILED on kid %u: %s\n",kids[i],strerror(result));
		    badcount++;
		  }		  
		}
	}

	for (i=0; i<NTHREADS; i++) {

	  result = thread_join(kids[i], &kid_result);

	  if (result) { /* join failed */
	    if (i%2) { /* These are the detached threads, join SHOULD fail */
	      kprintf("\t CORRECT behavior - thread_join on detached kid %u failed: %s\n", kids[i],strerror(result));
	    } else {
	      kprintf("\t ERROR - thread_join failed on non-detached kid %u: %s\n",kids[i],strerror(result));
	      badcount++;
	    }

	  } else { /* join succeeded */
	    if (i%2) { /* but it shouldn't for detached threads */
	      kprintf("\t ERROR - thread_join succeeded but kid %u was detached\n",kids[i]);
	      badcount++;
	    } else {
	      kprintf("\t Got exit status %d from kid %u\n",kid_result,kids[i]);
	      if (kid_result == i) {
		kprintf("\t\t SUCCESS!\n");
	      } else {
		kprintf("\t\t FAILED: should have been %d\n",i);
		badcount++;
	      }
	    }
	  }
	}

	kprintf("Join test 2 done - %d failures\n", badcount);
	return 0;
}

