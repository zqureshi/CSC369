/*
 * Test code for coremap page allocation.
 */
#include <types.h>
#include <lib.h>
#include <synch.h>
#include <thread.h>
#include <test.h>
#include <vm.h>
#include <machine/coremap.h>

/*
 * Test alloc_kpages; allocate NPAGES pages NTRIES times, freeing
 * somewhat later.
 *
 * The total of NPAGES * NTRIES is intended to exceed the size of
 * available memory.
 *
 * coremapstress does the same thing, but from NTHREADS different
 * threads at once.
 */

#define NTRIES   1200
#define NPAGES    3
#define NTHREADS  7

static
void
coremapthread(void *sm, unsigned long num)
{
	struct semaphore *sem = sm;
	u_int32_t page;
	u_int32_t oldpage = 0;
	u_int32_t oldpage2 = 0;
	int i;

	for (i=0; i<NTRIES; i++) {
		page = alloc_kpages(NPAGES);
		if (page==0) {
			if (sem) {
				kprintf("thread %lu: alloc_kpages failed\n",
					num);
				V(sem);
				return;
			}
			kprintf("alloc_kpages failed; test failed.\n");
			return;
		}
		if (oldpage2) {
			free_kpages(oldpage2);
		}
		oldpage2 = oldpage;
		oldpage = page;
	}
	if (oldpage2) {
		free_kpages(oldpage2);
	}
	if (oldpage) {
		free_kpages(oldpage);
	}
	if (sem) {
		V(sem);
	}
}

int
coremaptest(int nargs, char **args)
{
	(void)nargs;
	(void)args;

	kprintf("Starting kcoremap test...\n");
	coremapthread(NULL, 0);
	kprintf("kcoremap test done\n");

	return 0;
}

int
coremapstress(int nargs, char **args)
{
	struct semaphore *sem;
	int i, err;

	(void)nargs;
	(void)args;

	sem = sem_create("coremapstress", 0);
	if (sem == NULL) {
		panic("coremapstress: sem_create failed\n");
	}

	kprintf("Starting kcoremap stress test...\n");

	for (i=0; i<NTHREADS; i++) {
		err = thread_fork("coremapstress", sem, i, 
				  coremapthread, NULL);
		if (err) {
			panic("coremapstress: thread_fork failed (%d)\n", err);
		}
	}

	for (i=0; i<NTHREADS; i++) {
		P(sem);
	}

	sem_destroy(sem);
	kprintf("kcoremap stress test done\n");

	return 0;
}
