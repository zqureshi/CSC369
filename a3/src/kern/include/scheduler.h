#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

/*
 * Scheduler-related function calls.
 *
 *     scheduler     - run the scheduler and choose the next thread to run.
 *     make_runnable - add the specified thread to the run queue. If it's
 *                     already on the run queue or sleeping, weird things
 *                     may happen. Returns an error code.
 *
 *     print_run_queue - dump the run queue to the console for debugging.
 *
 *     scheduler_bootstrap - initialize scheduler data 
 *                           (must happen early in boot)
 *     scheduler_shutdown -  clean up scheduler data
 *     scheduler_preallocate - ensure space for at least NUMTHREADS threads.
 *                           Returns an error code.
 */

struct thread;

struct thread *scheduler(void);
int make_runnable(struct thread *t);

void print_run_queue(void);

void scheduler_bootstrap(void);
int scheduler_preallocate(int numthreads);
void scheduler_killall(void);
void scheduler_shutdown(void);

#endif /* _SCHEDULER_H_ */
