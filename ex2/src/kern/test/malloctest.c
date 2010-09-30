/*
 * Test code for kmalloc.
 */
#include <types.h>
#include <lib.h>
#include <synch.h>
#include <thread.h>
#include <test.h>

/*
 * Test kmalloc; allocate ITEMSIZE bytes NTRIES times, freeing
 * somewhat later.
 *
 * The total of ITEMSIZE * NTRIES is intended to exceed the size of
 * available memory.
 *
 * mallocstress does the same thing, but from NTHREADS different
 * threads at once.
 */

#define NTRIES   1200
#define ITEMSIZE  997
#define NTHREADS  8

static
void
mallocthread(void *sm, unsigned long num)
{
	struct semaphore *sem = sm;
	void *ptr;
	void *oldptr=NULL;
	void *oldptr2=NULL;
	int i;

	for (i=0; i<NTRIES; i++) {
		ptr = kmalloc(ITEMSIZE);
		if (ptr==NULL) {
			if (sem) {
				kprintf("thread %lu: kmalloc returned NULL\n",
					num);
				V(sem);
				return;
			}
			kprintf("kmalloc returned null; test failed.\n");
			return;
		}
		if (oldptr2) {
			kfree(oldptr2);
		}
		oldptr2 = oldptr;
		oldptr = ptr;
	}
	if (oldptr2) {
		kfree(oldptr2);
	}
	if (oldptr) {
		kfree(oldptr);
	}
	if (sem) {
		V(sem);
	}
}

int
malloctest(int nargs, char **args)
{
	(void)nargs;
	(void)args;

	kprintf("Starting kmalloc test...\n");
	mallocthread(NULL, 0);
	kprintf("kmalloc test done\n");

	return 0;
}

int
mallocstress(int nargs, char **args)
{
	struct semaphore *sem;
	int i, result;

	(void)nargs;
	(void)args;

	sem = sem_create("mallocstress", 0);
	if (sem == NULL) {
		panic("mallocstress: sem_create failed\n");
	}

	kprintf("Starting kmalloc stress test...\n");

	for (i=0; i<NTHREADS; i++) {
		result = thread_fork("mallocstress", sem, i, mallocthread,
				     NULL);
		if (result) {
			panic("mallocstress: thread_fork failed: %s\n",
			      strerror(result));
		}
	}

	for (i=0; i<NTHREADS; i++) {
		P(sem);
	}

	sem_destroy(sem);
	kprintf("kmalloc stress test done\n");

	return 0;
}
