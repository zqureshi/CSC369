#include <types.h>
#include <lib.h>
#include <kern/unistd.h>
#include <kern/errno.h>
#include <synch.h>
#include <thread.h>
#include <cache.h>

/* In this test, we create some number of threads, each of which 
 * will use a distinct set of BLOCKS_PER_THREAD blocks of data.
 * The data blocks are all accessed through the shared cache, 
 * however.
 */

#define BLOCKS_PER_THREAD 20

static struct semaphore *tsem = NULL;

static
void
init_sem(void)
{
	if (tsem==NULL) {
		tsem = sem_create("buffer test sem", 0);
		if (tsem == NULL) {
			panic("buffertest1: sem_create failed\n");
		}
	}
}

static void 
use_blocks(void *junk, unsigned long me)
{
  int i; /* index variable to loop over each block */
  int j; /* index variable to loop over each int in block */

  int my_first_id = me*BLOCKS_PER_THREAD; /* each thread uses unique blocks */
  int my_min_value = me*100000; /* each thread writes unique values */
  int value;
  int ints_per_buf = BUFSIZE/sizeof(int);

  int *my_buf = (int *)kmalloc(BUFSIZE);

  (void)junk;
	
  /* 1. Store some stuff into my blocks.  These should
   *    all go straight to backing store, since none 
   *    of these blocks have been read yet, and so will
   *    not be in the cache.
   */

  DEBUG(DB_THREADS,"Buftest thread %ld initializing my blocks...\n",me);

  for (i = 0; i < BLOCKS_PER_THREAD; i++) {
    value = my_min_value + i*1000;
    for (j = 0; j < ints_per_buf; j++) {
      my_buf[j] = value + j;
    }
    cache_write(my_first_id+i, (void *)my_buf);
  }


  /* 2. Read all the blocks and verify that they contain
   *    what we stored in them originally.
   */

  DEBUG(DB_THREADS,"Buftest thread %ld re-reading my blocks...\n",me);

  for (i = 0; i < BLOCKS_PER_THREAD; i++) {
    value = my_min_value + i*1000;
    cache_read(my_first_id+i, (void *)my_buf);
    for (j = 0; j < ints_per_buf; j++) {
      if (my_buf[j] != value + j) {
	kprintf("Error in Buffertest thread %ld: Block %d contains %d, expected %d\n",me, i, my_buf[j], value+j);
	goto done_buftest;
      }
    }
  }


  /* 3. Go through all the blocks again and modify them, so 
   *    they will be dirty when they are evicted from the cache.
   */

  DEBUG(DB_THREADS,"Buftest thread %ld reading and modifying my blocks...\n",me);

  for (i = 0; i < BLOCKS_PER_THREAD; i++) {
    value = my_min_value + i*1000;
    cache_read(my_first_id+i, (void *)my_buf);
    for (j = 0; j < ints_per_buf; j++) {
      /* Check current value again */
      if (my_buf[j] != value + j) {
	kprintf("Error in Buffertest thread %ld: Block %d contains %d, expected %d\n",me, i, my_buf[j], value+j);
	goto done_buftest;
      }
      my_buf[j] = my_buf[j] * 10;
    }
    cache_write(my_first_id+i, (void *)my_buf);
  }


  /* 4. Read blocks one more time to confirm that previous writes 
   *    were all done correctly.
   */

  DEBUG(DB_THREADS,"Buftest thread %ld re-reading my blocks final time...\n",me);

  for (i = 0; i < BLOCKS_PER_THREAD; i++) {
    value = my_min_value + i*1000;
    cache_read(my_first_id+i, (void *)my_buf);
    for (j = 0; j < ints_per_buf; j++) {
      if (my_buf[j] != (value + j)*10 ) {
	kprintf("Error in Buffertest thread %ld: Block %d contains %d, expected %d\n",me, i, my_buf[j], (value+j)*10);
	goto done_buftest;
      }
    }
  }
 

done_buftest:
  V(tsem);

}

static
void
runthreads(int nthreads)
{
	char name[16];
	int i, result;

	for (i=0; i<nthreads; i++) {
		snprintf(name, sizeof(name), "buffertest1_%d", i);
		result = thread_fork(name, NULL, i, use_blocks, NULL);
		if (result) {
			panic("buffertest1: thread_fork failed %s)\n", 
			      strerror(result));
		}
	}

	for (i=0; i<nthreads; i++) {
		P(tsem);
	}
}

int buffertest1(int nargs, char **args)
{
  (void)nargs;
  (void)args;

  init_sem();

  kprintf("Starting buffer test 1. ");

  if (nargs==1) {
    kprintf("Using 1 thread. (Specify buftest1 [nthreads] to use more threads)\n");
    use_blocks(NULL, 0);
  }
  else if (nargs==2) {
    kprintf("Using %d threads.\n",atoi(args[1]));
    runthreads(atoi(args[1]));
  }
  else {
    kprintf("Usage: buftest1 [nthreads]\n");
    kprintf("If nthreads is not specified, the test is run by the menu thread\n");
    return 1;
  }

  kprintf("Done buffer test 1.\n");

  return 0;

}

