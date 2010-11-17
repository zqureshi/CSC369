/*
 * Test multiple user level threads inside a process. The program
 * forks 3 threads off 2 to functions, each of which displays a string
 * every once in a while.
 *
 * This won't do much of anything unless you implement user-level
 * threads.
 *
 * It also makes various assumptions about the thread API. In
 * particular, it believes (1) that you create a thread by calling
 * "threadfork()" and passing the address for execution of the new
 * thread to begin at, (2) that if the parent thread exits any child
 * threads will keep running, and (3) child threads will exit if they
 * return from the function they started in. If any or all of these
 * assumptions are not met by your user-level threads, you will need
 * to patch this test accordingly.
 *
 * This is also a rather basic test and you'll probably want to write
 * some more of your own.
 */


#include <unistd.h>
#include <stdio.h>

#define NTHREADS  3
#define MAX       1<<25

/* counter for the loop in the threads : 
   This variable is shared and incremented by each 
   thread during his computation */
volatile int count = 0;

/* the 2 threads : */
void ThreadRunner(void);
void BladeRunner(void);

int
main(int argc, char *argv[])
{
    int i;

    (void)argc;
    (void)argv;

    for (i=0; i<NTHREADS; i++) {
	if (i)
	    threadfork(ThreadRunner);
        else
	    threadfork(BladeRunner);
    }

    printf("Parent has left.\n");
    return 0;
}

/* multiple threads will simply print out the global variable.
   Even though there is no synchronization, we should get some 
   random results.
*/

void
BladeRunner()
{
    while (count < MAX) {
	if (count % 500 == 0)
	    printf("Blade ");
	count++;
    }
}

void
ThreadRunner()
{
    while (count < MAX) {
	if (count % 513 == 0)
	    printf(" Runner\n");
	count++;
    }
}
    
