/*
 * Process ID managment.
 * File new for ASST1.
 */

#ifndef _PID_H_
#define _PID_H_

#define INVALID_PID	0	/* nothing has this pid */
#define BOOTUP_PID	1	/* first thread has this pid */

/*
 * Initialize pid management.
 * Initializes the process id table and also  
 * creates and initializes a structure to manage info associated
 * with the bootup thread.
 */
void pid_bootstrap(void);

/*
 * Get a pid for a new thread.
 * Returns 0 on success, or an error code as the result of the 
 * function.  Returns the newly-allocated process id in retval.
 */
int pid_alloc(pid_t *retval);

/*
 * Undo pid_alloc (may blow up if the target has ever run)
 */
void pid_unalloc(pid_t targetpid);

/* ASST1 Soln: New functions to handle exit/join/detach */

/* Mark the pid for a thread as exited */
void pid_setexited(pid_t donepid, int exitcode);

/* Mark a thread as not joinable */
int pid_detach(pid_t who);

/* Wait for a thread to finish */
int pid_join(pid_t who, int *status);

#endif /* _PID_H_ */
