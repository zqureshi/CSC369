#include <types.h>
#include <lib.h>
#include <queue.h>
#include <test.h>

static
void
testq(struct queue *q, int n)
{
	int i, result, *x, *r;

	x = kmalloc(n * sizeof(int));
	for (i=0; i<n; i++) {
		x[i] = i;
	}

	assert(q_empty(q));

	for (i=0; i<n; i++) {
		kprintf("queue: adding %d\n", i);
		result = q_addtail(q, &x[i]);
		assert(result==0);
	}

	for (i=0; i<n; i++) {
		r = q_remhead(q);
		assert(r != NULL);
		kprintf("queue: got %d, should be %d\n", *r, i);
		assert(*r == i);
	}

	assert(q_empty(q));

	kfree(x);
}


int
queuetest(int nargs, char **args)
{
	struct queue *q;

	(void)nargs;
	(void)args;

	q = q_create(8);
	assert(q != NULL);

	/* this doesn't require growing the queue */
	testq(q, 6);

	/* this requires growing the queue */
	testq(q, 27);

	q_destroy(q);

	return 0;
}
