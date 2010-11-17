/*
 * Process ID management.
 * File new for ASST1
 */

#include <types.h>
#include <kern/errno.h>
#include <kern/limits.h>
#include <kern/unistd.h>
#include <lib.h>
#include <array.h>
#include <queue.h>
#include <clock.h>
#include <thread.h>
#include <curthread.h>
#include <synch.h>
#include <pid.h>

/*
 * Structure for holding some data for a thread.
 *
 */
struct pidinfo {
	int pi_pid;			// process id of this thread
	int pi_ppid;			// process id of parent thread
	volatile int pi_exited;		// true if thread has exited
	int pi_exitstatus;		// status (only valid if exited)
        int pi_joinable;                // true if thread is joinable
        struct cv *pi_cv;               // for parent to wait for child exit
};


/*
 * Global pid and exit data.
 *
 * The process table is an el-cheapo hash table. It's indexed by
 * (pid % PROCS_MAX), and only allows one process per slot. If a
 * new pid allocation would cause a hash collision, we just don't
 * use that pid.
 */

static struct lock *pi_lock;            // lock for all pid ops

static struct pidinfo *pidinfo[PROCS_MAX]; // actual pid info
static pid_t nextpid;			// next candidate pid
static int nprocs;			// number of allocated pids


/*
 * Create a pidinfo structure for the specified pid.
 */
static
struct pidinfo *
pidinfo_create(pid_t pid, pid_t ppid)
{
	struct pidinfo *pi;

	assert(pid != INVALID_PID);

	pi = kmalloc(sizeof(struct pidinfo));
	if (pi==NULL) {
		return NULL;
	}

	/* ASST1: Create a condition var for this pid's parent
	 * to wait for it to exit.
	 */
	pi->pi_cv = cv_create("pid cv");
	if (pi->pi_cv == NULL) {
	  kfree(pi);
	  return NULL;
	}

	pi->pi_pid = pid;
	pi->pi_ppid = ppid;
	pi->pi_exited = FALSE;
	pi->pi_exitstatus = 0xbeef;  /* Recognizable unlikely exit code */
	pi->pi_joinable = TRUE; /* Threads start out joinable by default */
	return pi;
}

/*
 * Clean up a pidinfo structure.
 */
static
void
pidinfo_destroy(struct pidinfo *pi)
{
	assert(pi->pi_exited==TRUE);
	assert(pi->pi_ppid==INVALID_PID);
	cv_destroy(pi->pi_cv);  /* ASST1: destroy cv we created for this pid */
	kfree(pi);
}

////////////////////////////////////////////////////////////

/*
 * pid_bootstrap: initialize.
 */
void
pid_bootstrap(void)
{
	int i;

	/* ASST1: Initialize lock on pidinfo table */
	if ( (pi_lock = lock_create("pidinfo table lock")) == NULL) {
	  panic("Failed to create lock for pidinfo table");
	}

	/* not really necessary - should start zeroed */
	for (i=0; i<PROCS_MAX; i++) {
		pidinfo[i] = NULL;
	}

	pidinfo[BOOTUP_PID] = pidinfo_create(BOOTUP_PID, INVALID_PID);
	if (pidinfo[BOOTUP_PID]==NULL) {
		panic("Out of memory creating bootup pid data\n");
	}

	nextpid = PID_MIN;
	nprocs = 1;
}

/*
 * pi_get: look up a pidinfo in the process table.
 */
static
struct pidinfo *
pi_get(pid_t pid)
{
	struct pidinfo *pi;

	assert(pid>=0);
	assert(pid != INVALID_PID);

	pi = pidinfo[pid % PROCS_MAX];
	if (pi==NULL) {
		return NULL;
	}
	if (pi->pi_pid != pid) {
		return NULL;
	}
	return pi;
}

/*
 * pi_put: insert a new pidinfo in the process table. The right slot
 * must be empty.
 */
static
void
pi_put(pid_t pid, struct pidinfo *pi)
{

	assert(pid != INVALID_PID);

	assert(pidinfo[pid % PROCS_MAX] == NULL);
	pidinfo[pid % PROCS_MAX] = pi;
	nprocs++;
}

/*
 * pi_drop: remove a pidinfo structure from the process table and free
 * it. It should reflect a process that has already exited and been
 * waited for.
 */
static
void
pi_drop(pid_t pid)
{
	struct pidinfo *pi;

	pi = pidinfo[pid % PROCS_MAX];
	assert(pi != NULL);
	assert(pi->pi_pid == pid);

	pidinfo_destroy(pi);
	pidinfo[pid % PROCS_MAX] = NULL;
	nprocs--;
}

////////////////////////////////////////////////////////////

/*
 * Helper function for pid_alloc.
 */
static
void
inc_nextpid(void)
{

	nextpid++;
	if (nextpid > PID_MAX) {
		nextpid = PID_MIN;
	}
}

/*
 * pid_alloc: allocate a process id.
 */
int
pid_alloc(pid_t *retval)
{
	struct pidinfo *pi;
	pid_t pid;
	int count;

	assert(curthread->t_pid != INVALID_PID);

	/* ASST1: Lock the pidinfo table */
	lock_acquire(pi_lock);

	if (nprocs == PROCS_MAX) {
		lock_release(pi_lock); /* ASST1: release lock before return */
		return EAGAIN;
	}

	/*
	 * The above test guarantees that this loop terminates, unless
	 * our nprocs count is off. Even so, assert we aren't looping
	 * forever.
	 */
	count = 0;
	while (pidinfo[nextpid % PROCS_MAX] != NULL) {

		/* avoid various boundary cases by allowing extra loops */
		assert(count < PROCS_MAX*2+5);
		count++;

		inc_nextpid();
	}

	pid = nextpid;

	pi = pidinfo_create(pid, curthread->t_pid);
	if (pi==NULL) {
		lock_release(pi_lock); /* ASST1: release lock before return */
		return ENOMEM;
	}

	pi_put(pid, pi);

	inc_nextpid();

	lock_release(pi_lock); /* ASST1: release lock before return */

	*retval = pid;
	return 0;
}

/*
 * pid_unalloc - unallocate a process id (allocated with pid_alloc) that
 * hasn't run yet.
 */
void
pid_unalloc(pid_t theirpid)
{
	struct pidinfo *them;

	assert(theirpid >= PID_MIN && theirpid <= PID_MAX);

	lock_acquire(pi_lock); /* ASST1: Lock the pidinfo table */

	them = pi_get(theirpid);
	assert(them != NULL);
	assert(them->pi_exited==FALSE);
	assert(them->pi_ppid==curthread->t_pid);

	/* keep pidinfo_destroy from complaining */
	them->pi_exitstatus = 0xdead;
	them->pi_exited = TRUE;
	them->pi_ppid = INVALID_PID;

	pi_drop(theirpid);

	lock_release(pi_lock);  /* ASST1: release lock before return */

}

/*** Remaining functions all new as part of ASST1 solution ***/

/*
 * pid_setexited: Set the exitstatus for a pid.
 * Synchronize with parent if thread is not detached already.
 */
void
pid_setexited(pid_t donepid, int exitcode)
{
	struct pidinfo *pi_done;
	int i;

	assert(donepid >= PID_MIN && donepid <= PID_MAX);

	lock_acquire(pi_lock); /* Lock the pidinfo table */

	/* First, detach all children. (If parent is exiting,
	 * it clearly will never wait for its children.)
	 * We could use pid_detach here, but we don't really
	 * need all the usage checks, and we already hold the
	 * pi_lock, so it is simpler to just do the work
	 * of pid_detach here.
	 */
	for (i=0; i<PROCS_MAX; i++) {
		if (pidinfo[i]==NULL) {
			continue;
		}
		if (pidinfo[i]->pi_ppid == donepid) {
			pidinfo[i]->pi_ppid = INVALID_PID;
			pidinfo[i]->pi_joinable = FALSE;
			if (pidinfo[i]->pi_exited) {
				pi_drop(pidinfo[i]->pi_pid);
			}
		}
	}

	/* Now mark the specified thread as done */

	pi_done = pi_get(donepid);
	assert(pi_done != NULL);
	assert(pi_done->pi_exited==0);

	pi_done->pi_exitstatus = exitcode;
	pi_done->pi_exited = TRUE;

	/* synchronize: Is this thread detached already? */
	if (pi_done->pi_joinable) {
	  cv_signal(pi_done->pi_cv, pi_lock);
	} else {
	  // If no one is going to wait, we'd better clean up
	  pi_drop(donepid);
	}

	lock_release(pi_lock);  /* release lock before return */

}

/* Mark a thread as not joinable.  Free pidinfo struct if thread
 * being detached has already exited. 
 */
int pid_detach(pid_t who)
{
	struct pidinfo *pi_who;

	if (who < PID_MIN || who > PID_MAX) {
		return EINVAL;
	}
	
	lock_acquire(pi_lock); /* Lock the pidinfo table */
	
	pi_who = pi_get(who);
	
	// Usage checks:
	if (pi_who == NULL) {
		lock_release(pi_lock);  /* release lock before return */
		return ESRCH;
	}
	
	if (pi_who->pi_ppid != curthread->t_pid) {
		lock_release(pi_lock);  /* release lock before return */
		return EINVAL;
	}
	
	if (pi_who->pi_joinable != TRUE) {
		// thread is already detached 
		lock_release(pi_lock);  /* release lock before return */
		return EINVAL;
	}

	/* Mark it not joinable. */
	pi_who->pi_joinable = FALSE;
	pi_who->pi_ppid = INVALID_PID;

	/* Has thread exited already? */
	if (pi_who->pi_exited == TRUE) {
		/* then clean up the pid struct */		
		pi_drop(who);
	}

	lock_release(pi_lock);  /* release lock before return */
	
	return 0;
}

/* wait for a thread to finish */
int pid_join(pid_t who, int *status)
{

	struct pidinfo *pi_who;

	if (who < PID_MIN || who > PID_MAX) {
		return EINVAL;
	}

	lock_acquire(pi_lock); /* Lock the pidinfo table */

	pi_who = pi_get(who);

	// Usage checks:
	if (pi_who == NULL) {
		lock_release(pi_lock);  /* release lock before return */
		return ESRCH;
	}

	if (pi_who->pi_ppid != curthread->t_pid) {
		lock_release(pi_lock);  /* release lock before return */
		return EINVAL;
	}

	if (pi_who->pi_joinable != TRUE) {
		/* thread is already detached, can't join */
		lock_release(pi_lock);  /* release lock before return */
		return EINVAL;
	}

	/* Has thread exited already? */
	if (pi_who->pi_exited == FALSE) {
		/* wait for it */
		cv_wait(pi_who->pi_cv, pi_lock);
	}

	/*  Has to be exited now. */
	assert(pi_who->pi_exited == TRUE);
	*status = pi_who->pi_exitstatus;

	/* Don't need the pid info anymore. */
	pi_who->pi_ppid = INVALID_PID; /* Keep pi_drop happy */
	pi_drop(who);

	lock_release(pi_lock);  /* release lock before return */

	return 0;
}

