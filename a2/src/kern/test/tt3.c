/*
 * More thread test code.
 */
#include <types.h>
#include <lib.h>
#include <machine/spl.h>
#include <synch.h>
#include <thread.h>
#include <test.h>

/* dimension of matrices (cannot be too large or will overflow stack) */

#if OPT_SYNCHPROBS
#define DIM 10
#else
#define DIM 64
#endif

/* number of iterations for sleepalot threads */
#define SLEEPALOT_PRINTS      20	/* number of printouts */
#define SLEEPALOT_ITERS       4		/* iterations per printout */
/* polling frequency of waker thread */
#define WAKER_WAKES          100
/* number of iterations per compute thread */
#define COMPUTE_ITERS         10

#define NWAITADDRS 12
static const char waitaddrs[NWAITADDRS];  /* N distinct wait addresses */

static volatile int wakerdone;
static struct semaphore *wakersem;
static struct semaphore *donesem;

static
void
setup(void)
{
	if (wakersem == NULL) {
		wakersem = sem_create("wakersem", 1);
	}
	if (donesem == NULL) {
		donesem = sem_create("donesem", 0);
	}
	wakerdone = 0;
}

static
void
sleepalot_thread(void *junk, unsigned long num)
{
	int i, j, spl;

	(void)junk;

	for (i=0; i<SLEEPALOT_PRINTS; i++) {
		for (j=0; j<SLEEPALOT_ITERS; j++) {
			spl = splhigh();
			thread_sleep(&waitaddrs[random()%NWAITADDRS]);
			splx(spl);
		}
		kprintf("[%lu]", num);
	}
	V(donesem);
}

static
void
waker_thread(void *junk1, unsigned long junk2)
{
	int i, spl, done;

	(void)junk1;
	(void)junk2;

	while (1) {
		P(wakersem);
		done = wakerdone;
		V(wakersem);
		if (done) {
			break;
		}

		for (i=0; i<WAKER_WAKES; i++) {
			spl = splhigh();
			thread_wakeup(&waitaddrs[random()%NWAITADDRS]);
			splx(spl);

			thread_yield();
		}
	}
	V(donesem);
}

static
void
make_sleepalots(int howmany)
{
	char name[16];
	int i, result;

	for (i=0; i<howmany; i++) {
		snprintf(name, sizeof(name), "sleepalot%d", i);
		result = thread_fork(name, NULL, i, sleepalot_thread, NULL);
		if (result) {
			panic("thread_fork failed: %s\n", strerror(result));
		}
	}
	result = thread_fork("waker", NULL, 0, waker_thread, NULL);
	if (result) {
		panic("thread_fork failed: %s\n", strerror(result));
	}
}

static
void
compute_thread(void *junk1, unsigned long num)
{
	struct matrix {
		char m[DIM][DIM];
	};
	struct matrix *m1, *m2, *m3;
	unsigned char tot;
	int i, j, k, m;
	u_int32_t rand;

	(void)junk1;

	m1 = kmalloc(sizeof(struct matrix));
	assert(m1 != NULL);
	m2 = kmalloc(sizeof(struct matrix));
	assert(m2 != NULL);
	m3 = kmalloc(sizeof(struct matrix));
	assert(m3 != NULL);

	for (m=0; m<COMPUTE_ITERS; m++) {

		for (i=0; i<DIM; i++) {
			for (j=0; j<DIM; j++) {
				rand = random();
				m1->m[i][j] = rand >> 16;
				m2->m[i][j] = rand & 0xffff;
			}
		}
		
		for (i=0; i<DIM; i++) {
			for (j=0; j<DIM; j++) {
				tot = 0;
				for (k=0; k<DIM; k++) {
					tot += m1->m[i][k] * m2->m[k][j];
				}
				m3->m[i][j] = tot;
			}
		}
		
		tot = 0;
		for (i=0; i<DIM; i++) {
			tot += m3->m[i][i];
		}

		kprintf("{%lu: %u}", num, (unsigned) tot);
		thread_yield();
	}

	kfree(m1);
	kfree(m2);
	kfree(m3);

	V(donesem);
}

static
void
make_computes(int howmany)
{
	char name[16];
	int i, result;

	for (i=0; i<howmany; i++) {
		snprintf(name, sizeof(name), "compute%d", i);
		result = thread_fork(name, NULL, i, compute_thread, NULL);
		if (result) {
			panic("thread_fork failed: %s\n", strerror(result));
		}
	}
}

static
void
finish(int howmanytotal)
{
	int i;
	for (i=0; i<howmanytotal; i++) {
		P(donesem);
	}
	P(wakersem);
	wakerdone = 1;
	V(wakersem);
	P(donesem);
}

static
void
runtest3(int nsleeps, int ncomputes)
{
	setup();
	kprintf("Starting thread test 3 (%d [sleepalots], %d {computes}, "
		"1 waker)\n",
		nsleeps, ncomputes);
	make_sleepalots(nsleeps);
	make_computes(ncomputes);
	finish(nsleeps+ncomputes);
	kprintf("\nThread test 3 done\n");
}

int
threadtest3(int nargs, char **args)
{
	if (nargs==1) {
		runtest3(5, 2);
	}
	else if (nargs==3) {
		runtest3(atoi(args[1]), atoi(args[2]));
	}
	else {
		kprintf("Usage: tt3 [sleepthreads computethreads]\n");
		return 1;
	}
	return 0;
}
