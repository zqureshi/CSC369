/*
 * Array of void pointers. See array.h.
 */
#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <array.h>

struct array {
	int num;
	int max;
	void **v;
};

struct array *
array_create(void)
{
	struct array *a = kmalloc(sizeof(struct array));
	if (a==NULL) {
		return NULL;
	}
	a->v = NULL;
	a->num = 0;
	a->max = 0;
	return a;
}

int
array_getnum(struct array *a)
{
	return a->num;
}

void *
array_getguy(struct array *a, int index)
{
	assert(a->num <= a->max);
	assert(index >=0 && index < a->num);
	return a->v[index];
}

int
array_preallocate(struct array *a, int nguys)
{
	void **newv;
	int i;
	int newmax = a->max;

	assert(a->num >=0 && a->num <= a->max);
		
	while (nguys > newmax) {
		newmax = (newmax+1)*2;
	}
	newv = kmalloc(newmax * sizeof(void *));
	if (newv==NULL) {
		return ENOMEM;
	}
	a->max = newmax;
	for (i=0; i<a->num; i++) newv[i] = a->v[i];
	if (a->v!=NULL) {
		kfree(a->v);
	}
	a->v = newv;
	return 0;
}

int
array_setsize(struct array *a, int nguys)
{
	int result;

	assert(a->num >=0 && a->num <= a->max);

	if (nguys > a->max) {
		result = array_preallocate(a, nguys);
		if (result) {
			return result;
		}
	}
	else if (nguys==0 && a->max > 16) {
		assert(a->v!=NULL);
		kfree(a->v);
		a->v = NULL;
		a->max = 0;
	}
	a->num = nguys;

	return 0;
}

void
array_setguy(struct array *a, int index, void *ptr)
{
	assert(a->num <= a->max);
	assert(index >=0 && index < a->num);
	a->v[index] = ptr;
}

int
array_add(struct array *a, void *guy)
{
	int ix, result;

	ix = a->num;

	result = array_setsize(a, ix+1);
	if (result) {
		return result;
	}

	a->v[ix] = guy;

	return 0;
}

void
array_remove(struct array *a, int index)
{
	int nmove;

	assert(a->num <= a->max);
	assert(index >=0 && index < a->num);

	nmove = a->num - (index + 1);
	memmove(a->v+index, a->v+index+1, nmove*sizeof(void *));
	a->num--;
}

void
array_destroy(struct array *a)
{
	if (a->v) kfree(a->v);
	kfree(a);
}
