#include <types.h>
#include <lib.h>
#include <kern/unistd.h>
#include <kern/errno.h>
#include <synch.h>
#include <thread.h>
#include <cache.h>

/* In this test, we will create the following threads:
 * a) One updater thread, which writes values to blocks accessed through
 *    the cache. Each block in this test will have a version number, followed
 *    by repeating (block number + version number) to fill out the block.
 * b) One checker thread, which repeatedly calls the external cache test
 *    function "check_cache_integrity().  The integrity check simply prints
 *    a message if two buffers have the same block id.
 * c) A variable number (set by a parameter when buffertest2 is called) of
 *    reader threads which read the same set of blocks from the cache, and 
 *    check to see that their contents is as expected.
 *
 * You will likely see a "convoy" effect, as each thread reads the same 
 * blocks in the same order, doing the same amount of work per block.
 * This test can be made more interesting by inserting a variable amount
 * of "work" per block.
 */

static struct semaphore *tsem = NULL;

static
void
init_sem(void)
{
	if (tsem==NULL) {
		tsem = sem_create("buffer test sem", 0);
		if (tsem == NULL) {
			panic("buffertest2: sem_create failed\n");
		}
	}
}

#define NBLOCKS 100
#define NTRIES 10

/* Simple synchronization for checker thread to decide when to stop.
 * We don't bother locking access to this shared variable, since it is
 * only modified by a single thread, and we don't care if the checker
 * does an extra iteration or two before noticing that "done" has been
 * set to TRUE.
 */

static volatile int done = FALSE;

static void 
check_cache(void *junk, unsigned long me)
{
  (void)junk;
  (void)me;

  while (!done) {
    check_cache_integrity();
    /*thread_yield();*/
    clocksleep(1);
  }
}

static void 
read_same_blocks(void *junk, unsigned long me)
{
  /* Each thread repeatedly reads the same set of NBLOCKS blocks,
   * Simple sanity check here is that each thread should see a
   * monotonically increasing "version number" in the blocks that it
   * reads.  Observing otherwise means that an update has been lost.
   */
  (void)junk;
  int i,j,k;
  int *my_buf = (int *)kmalloc(BUFSIZE);
  int last_vers[NBLOCKS];
  int ints_per_buf = BUFSIZE/sizeof(int);

  for (i = 0; i < NBLOCKS; i++) {
    last_vers[i] = 0;
  }

  for (i = 0; i < NTRIES; i++) {
    DEBUG(DB_TEST,"Thread %ld, doing trial %d\n",me,i);
    for (j = 0; j < NBLOCKS; j++) {
      cache_read(j, (void *)my_buf);      
      /* Check block read */
      if (my_buf[0] < last_vers[j]) {
	kprintf("ERROR in buftest2: thread %ld saw version %d before, now %d\n",
		me, last_vers[j], my_buf[0]);
      }
      last_vers[j] = my_buf[0];
      for (k = 1; k < ints_per_buf; k++) {
	if (my_buf[k] != j + last_vers[j]) {
	  kprintf("ERROR in buftest2: thread %ld sees block %d with version %d, value %d, not expected %d\n",me, j, my_buf[0], my_buf[k], j+last_vers[j]);
	  break;
	}
      }
    }
  }
    
  kfree(my_buf);
  V(tsem);
}

/* Function executed by updater thread.  Just increments the version
 * number at the start of each block, and then fills the block with
 * (block number + version number) before writing it back through the
 * cache.
 */

static void 
update_same_blocks(void *junk, unsigned long me)
{
  (void)junk;
  (void)me;

  int i,j,k;
  int *my_buf = (int *)kmalloc(BUFSIZE);
  int ints_per_buf = BUFSIZE/sizeof(int);

  for (i= 0; i < NTRIES; i++) {
    DEBUG(DB_TEST,"Updater thread doing version %d\n",i);
    for (j = 0; j  < NBLOCKS; j++) {
      my_buf[0] = i;
      for (k = 1; k < ints_per_buf; k++) {
	my_buf[k] = j + i;
      }
      cache_write(j, (void *)my_buf);
    }

    if (i==0) {
      /* Make sure blocks are initialized once before readers start */
      V(tsem);
      clocksleep(1);
    }
  }

  kfree(my_buf);
  V(tsem);

}

static
void
runthreads(int nthreads)
{
	char name[16];
	int i, result;

	result = thread_fork("bufchecker",NULL,i,check_cache,NULL);
	if (result) {
	  panic("buffertest2: thread_fork failed %s\n",
		strerror(result));
	}
      
	result = thread_fork("buftest2_writer", NULL, i, update_same_blocks,NULL);
	if (result) {
	  panic("buffertest2: thread_fork failed %s\n",
		strerror(result));
	}

	P(tsem);

	for (i=1; i<nthreads; i++) {
		snprintf(name, sizeof(name), "buffertest2_%d", i);
		result = thread_fork(name, NULL, i, read_same_blocks, NULL);
		if (result) {
			panic("buffertest2: thread_fork failed %s)\n", 
			      strerror(result));
		}
	}

	for (i=0; i<nthreads; i++) {
		P(tsem);
	}

	done = TRUE;
}

int buffertest2(int nargs, char **args)
{
  (void)nargs;
  (void)args;

  init_sem();

  kprintf("Starting buffer test 2...\n");

  if (nargs==1) {
    runthreads(1);
  }
  else if (nargs==2) {
    runthreads(atoi(args[1]));
  }
  else {
    kprintf("Usage: buftest2 [num reader threads]\n");
    kprintf("If nthreads is not specified, the test is run with one reader, one writer, and one checker thread\n");
    return 1;
  }

  kprintf("Done buffer test 2.\n");

  return 0;

}

