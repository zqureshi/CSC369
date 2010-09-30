/*
 * Queue of void pointers. See queue.h for details.
 */

#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <queue.h>

struct queue {
	int size;
	int nextwrite;	// next element to write to (was head)
	int nextread;	// next element to read from (was tail)
	void **data;
};

static
int
q_grow(struct queue *q, int targetsize)
{
	void **olddata = q->data;
	int onr = q->nextread;
	int onw = q->nextwrite;
	int osize = q->size;

	int nsize;
	void **ndata;

	int i, result;

	nsize = q->size;
	while (nsize < targetsize) {
		nsize *= 2;
		/* prevent infinite loop */
		assert(nsize > 0);
	}
	ndata = kmalloc(nsize * sizeof(void *));
	if (ndata == NULL) {
		return ENOMEM;
	}
	q->size = nsize;
	q->data = ndata;
	q->nextread = q->nextwrite = 0;
	
	for (i=onr; i!=onw; i = (i+1)%osize) {
		result = q_addtail(q, olddata[i]);
		assert(result==0);
	}
	kfree(olddata);
	return 0;
}

struct queue *
q_create(int size)
{
	struct queue *q = kmalloc(sizeof(struct queue));
	if (q==NULL) {
		return NULL;
	}
	q->size = size;
	q->data = kmalloc(size * sizeof(void *));
	if (q->data==NULL) {
		kfree(q);
		return NULL;
	}
	q->nextread = q->nextwrite = 0;
	return q;
}

int
q_preallocate(struct queue *q, int size)
{
	int result = 0;

	assert(q->size > 0);

	if (size > q->size) {
		result = q_grow(q, size);
	}
	return result;
}

inline
int
q_empty(struct queue *q)
{
	return q->nextwrite == q->nextread;
}

int
q_addtail(struct queue *q, void *ptr)
{
	int nextnext, result;

	assert(q->size > 0);

	nextnext = (q->nextwrite+1) % q->size;
	if (nextnext==q->nextread) {
		result = q_grow(q, q->size+1);
		if (result) {
			return result;
		}
		nextnext = (q->nextwrite+1) % q->size;
	}
	q->data[q->nextwrite] = ptr;
	q->nextwrite = nextnext;
	return 0;
}

void *
q_remhead(struct queue *q)
{
	void *ret;

	assert(q->size > 0);

	assert(!q_empty(q));
	ret = q->data[q->nextread];
	q->nextread = (q->nextread+1)%q->size;
	return ret;
}

void
q_destroy(struct queue *q)
{
	assert(q_empty(q));
	kfree(q->data);
	kfree(q);
}

/* These are really intended only for debugging. */
int
q_getstart(struct queue *q)
{
	return q->nextread;
}

int
q_getend(struct queue *q)
{
	return q->nextwrite;
}

int
q_getsize(struct queue *q)
{
	return q->size;
}

void *
q_getguy(struct queue *q, int index)
{
	// note that we don't check to make sure the access isn't in the
	// unused part of the queue space. we probably should.

	assert(index>=0 && index<q->size);
	return q->data[index];
}
