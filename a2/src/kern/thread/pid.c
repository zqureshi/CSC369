/*
 * Process ID management.
 * File new for ASST2
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

/* Lock for the pid monitor */
struct lock *pid_lock;

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
  struct cv *pi_join; //Condition on which parent waits to join
};


/*
 * Global pid and exit data.
 *
 * The process table is an el-cheapo hash table. It's indexed by
 * (pid % PROCS_MAX), and only allows one process per slot. If a
 * new pid allocation would cause a hash collision, we just don't
 * use that pid.
 */

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

	pi->pi_pid = pid;
	pi->pi_ppid = ppid;
	pi->pi_exited = FALSE;
	pi->pi_exitstatus = 0xbeef;  /* Recognizable unlikely exit code */
	pi->pi_joinable = TRUE; /* Threads start out joinable by default */
  pi->pi_join = cv_create("pi_join"); /* ppid waits on this */
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
  cv_destroy(pi->pi_join);
	kfree(pi);
}

////////////////////////////////////////////////////////////

/*
 * pid_bootstrap: initialize.
 */
void
pid_bootstrap(void)
{
  /* Create pid lock */
  pid_lock = lock_create("pid_lock");

	int i;

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
 * Cleanup pid
 */
void pid_shutdown(void){
  lock_destroy(pid_lock);
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
  lock_acquire(pid_lock);

	struct pidinfo *pi;
	pid_t pid;
	int count;

	assert(curthread->t_pid != INVALID_PID);


	if (nprocs == PROCS_MAX) {
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
		return ENOMEM;
	}

	pi_put(pid, pi);

	inc_nextpid();

	*retval = pid;

  lock_release(pid_lock);

	return 0;
}

/*
 * pid_unalloc - unallocate a process id (allocated with pid_alloc) that
 * hasn't run yet.
 */
void
pid_unalloc(pid_t theirpid)
{
  lock_acquire(pid_lock);

	struct pidinfo *them;

	assert(theirpid >= PID_MIN && theirpid <= PID_MAX);

	them = pi_get(theirpid);
	assert(them != NULL);
	assert(them->pi_exited==FALSE);
	assert(them->pi_ppid==curthread->t_pid);

	/* keep pidinfo_destroy from complaining */
	them->pi_exitstatus = 0xdead;
	them->pi_exited = TRUE;
	them->pi_ppid = INVALID_PID;

	pi_drop(theirpid);

  lock_release(pid_lock);
}

int pid_wait(pid_t pid){
  lock_acquire(pid_lock);

  struct pidinfo *pi = pi_get(pid);

  if(pi == NULL){
    return ESRCH; /* No thread with given pid */
  }

  cv_wait(pi->pi_join, pid_lock);
  lock_release(pid_lock);

  return 0;
}

int pid_exit(pid_t pid, int exitstatus){
  lock_acquire(pid_lock);

  struct pidinfo *pi = pi_get(pid);

  if(pi == NULL){
    return ESRCH; /* No thread with given pid */
  }

  /* Set exited to TRUE and store status */
  pi->pi_exited = TRUE;
  pi->pi_exitstatus = exitstatus;
  DEBUG(DB_THREADS, "PID %d exited with status %d\n", (int)pid, exitstatus);

  /* Signal if thread is joinable */
  if(pi->pi_joinable == TRUE){
    cv_broadcast(pi->pi_join, pid_lock);
  }

  lock_release(pid_lock);
  return 0;
}
