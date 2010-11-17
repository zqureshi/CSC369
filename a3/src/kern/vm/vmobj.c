#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <array.h>
#include <machine/spl.h>
#include <machine/coremap.h>
#include <addrspace.h>
#include <vm.h>
#include <vmpvt.h>

/*
 * vm_object operations.
 * NEW FILE FOR ASST2
 */


/*
 * vm_object_create: Allocate a new vm_object with nothing in it.
 * Returns: new vm_object on success, NULL on error.
 */
struct vm_object *
vm_object_create(size_t npages)
{
	struct vm_object *vmo;
	unsigned i;
	int result;

	result = swap_reserve(npages);
	if (result != 0) {
		return NULL;
	}

	vmo = kmalloc(sizeof(struct vm_object));
	if (vmo == NULL) {
		swap_unreserve(npages);
		return NULL;
	}

	vmo->vmo_lpages = array_create();
	if (vmo->vmo_lpages == NULL) {
		kfree(vmo);
		swap_unreserve(npages);
		return NULL;
	}

	vmo->vmo_base = 0xdeadbeef;		/* make sure these */
	vmo->vmo_lower_redzone = 0xdeafbeef;	/* get filled in later */

	/* add the requested number of zerofilled pages */
	result = array_setsize(vmo->vmo_lpages, npages);
	if (result) {
		array_destroy(vmo->vmo_lpages);
		kfree(vmo);
		swap_unreserve(npages);
		return NULL;
	}

	for (i=0; i<npages; i++) {
		array_setguy(vmo->vmo_lpages, i, NULL);
	}

	return vmo;
}

/*
 * vm_object_copy: clone a vm_object.
 *
 * Synchronization: None; lpage_copy does the hard stuff.
 */
int
vm_object_copy(struct vm_object *vmo, struct addrspace *newas,
	       struct vm_object **ret)
{
	struct vm_object *newvmo;

	struct lpage *newlp, *lp;
	int j, result;

	newvmo = vm_object_create(array_getnum(vmo->vmo_lpages));
	if (newvmo == NULL) {
		return ENOMEM;
	}

	newvmo->vmo_base = vmo->vmo_base;
	newvmo->vmo_lower_redzone = vmo->vmo_lower_redzone;

	for (j = 0; j < array_getnum(vmo->vmo_lpages); j++) {
		lp = array_getguy(vmo->vmo_lpages, j);
		newlp = array_getguy(newvmo->vmo_lpages, j);

		/* new guy should be initialized to all zerofill */
		assert(newlp == NULL);

		if (lp == NULL) {
			/* old guy is zerofill too, don't do anything */
			continue;
		}

		result = lpage_copy(lp, &newlp);
		if (result) {
			goto fail;
		}
		array_setguy(newvmo->vmo_lpages, j, newlp);
	}

	*ret = newvmo;
	return 0;

 fail:
	vm_object_destroy(newas, newvmo);
	return result;
}

/*
 * vm_object_setsize: change the size of a vm_object.
 *
 * Synchronization: raise spl while freeing pages, so we can call mmu_unmap.
 */
int
vm_object_setsize(struct addrspace *as, struct vm_object *vmo, int npages)
{
	int i, spl, result;
	struct lpage *lp;

	assert(vmo != NULL);
	assert(vmo->vmo_lpages != NULL);

	if (npages < array_getnum(vmo->vmo_lpages)) {
		spl = splhigh();
		for (i=npages; i<array_getnum(vmo->vmo_lpages); i++) {
			lp = array_getguy(vmo->vmo_lpages, i);
			if (lp != NULL) {
				assert(as != NULL);
				/* remove any tlb entry for this mapping */
				mmu_unmap(as, vmo->vmo_base+PAGE_SIZE*i);
				lpage_destroy(lp);
			}
			else {
				swap_unreserve(1);
			}
		}
		splx(spl);
		result = array_setsize(vmo->vmo_lpages, npages);
		/* shrinking an array shouldn't fail */
		assert(result==0);
	}
	else if (npages > array_getnum(vmo->vmo_lpages)) {
		int oldsize = array_getnum(vmo->vmo_lpages);
		unsigned newpages = npages - oldsize;

		result = swap_reserve(newpages);
		if (result) {
			return result;
		}

		result = array_setsize(vmo->vmo_lpages, npages);
		if (result) {
			swap_unreserve(newpages);
			return result;
		}
		for (i=oldsize; i<npages; i++) {
			array_setguy(vmo->vmo_lpages, i, NULL);
		}
	}
	return 0;
}

/*
 * vm_object_destroy: Deallocates a vm_object.
 *
 * Synchronization: none; assumes one thread uniquely owns the object.
 */
void 					
vm_object_destroy(struct addrspace *as, struct vm_object *vmo)
{
	int result;

	result = vm_object_setsize(as, vmo, 0);
	assert(result==0);
	
	array_destroy(vmo->vmo_lpages);
	kfree(vmo);
}
