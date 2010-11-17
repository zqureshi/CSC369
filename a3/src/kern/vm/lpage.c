#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <machine/spl.h>
#include <synch.h>
#include <thread.h>
#include <machine/coremap.h>
#include <addrspace.h>
#include <vm.h>
#include <vmpvt.h>

/* 
 * lpage operations
 * New file for ASST2.
 */

/* Stats counters */
static volatile u_int32_t ct_zerofills;
static volatile u_int32_t ct_minfaults;
static volatile u_int32_t ct_majfaults;
static volatile u_int32_t ct_discard_evictions;
static volatile u_int32_t ct_write_evictions;

void
vm_printstats(void)
{
	int spl;
	u_int32_t zf, mn, mj, de, we, te;
	
	spl = splhigh();
	zf = ct_zerofills;
	mn = ct_minfaults;
	mj = ct_majfaults;
	de = ct_discard_evictions;
	we = ct_write_evictions;
	splx(spl);

	te = de+we;

	kprintf("vm: %lu zerofills %lu minorfaults %lu majorfaults\n",
		(unsigned long) zf, (unsigned long) mn, (unsigned long) mj);
	kprintf("vm: %lu evictions (%lu discarding, %lu writes)\n",
		(unsigned long) te, (unsigned long) de, (unsigned long) we);
}

/*
 * Create a logical page object.
 * Synchronization: none.
 */
struct lpage *
lpage_create(void)
{
	struct lpage *lp;

	lp = kmalloc(sizeof(struct lpage));
	if (lp==NULL) {
		return NULL;
	}

	lp->lp_swapaddr = INVALID_SWAPADDR;
	lp->lp_paddr = INVALID_PADDR;

	return lp;
}

/*
 * lpage_destroy: deallocates a logical page. Releases any RAM or swap
 * pages involved.
 *
 * Synchronization: Someone might be in the process of evicting the
 * page if it's resident, so it might be pinned. If so, wait for it
 * to become unpinned, and only then free it if still necessary.
 * This is gross and really ought to be done some better way. (XXX)
 *
 * We assume that lpages are not shared between address spaces and
 * address spaces are not shared between threads.
 */
void 					
lpage_destroy(struct lpage *lp)
{
	paddr_t pa;
	int spl;

	assert(lp != NULL);

	lpage_lock(lp);
	spl = splhigh();

	pa = lp->lp_paddr & PAGE_FRAME;
	if (pa != INVALID_PADDR) {
		lpage_unlock(lp);
		coremap_pin(pa);
		lpage_lock(lp);
		if ((lp->lp_paddr & PAGE_FRAME) == pa) {
			DEBUG(DB_VM, "lpage_destroy: freeing paddr 0x%x\n",
			      pa);
			coremap_free(pa, 0 /* iskern */);
		}
		else {
			assert((lp->lp_paddr & PAGE_FRAME) == INVALID_PADDR);
		}
		coremap_unpin(pa);
	}

	splx(spl);
	lpage_unlock(lp);

	/* assert(lp->lp_dpage!=INVALID_DPAGE); -- probably not true */
	if (lp->lp_swapaddr != INVALID_SWAPADDR) {
		DEBUG(DB_VM, "lpage_destroy: freeing swap addr 0x%x\n", 
		      lp->lp_swapaddr);
		swap_free(lp->lp_swapaddr);
	}

	kfree(lp);
}


/*
 * lpage_lock & lpage_unlock
 *
 * A logical page may be accessed by more than one thread: not only
 * the thread that owns it, but also the pager thread if such a thing
 * should exist, plus anyone else who might be swapping the page out.
 *
 * Therefore, it needs to be locked for usage. To avoid ballooning
 * memory usage, we use a spare bit in the paddr field instead of
 * a struct lock.
 *
 * Because one needs to hold the lock to change the other bits of
 * the paddr field, it should be safe to do this even if the paddr
 * field is not accessed atomically by the processor - just as long
 * as the lock bit is accessed atomically, which should be the case.
 * (Nonetheless, it might be better to rearrange this later. FUTURE)
 *
 * It is more or less incorrect to wait on this lock for any great
 * length of time.
 * 
 *      lpage_lock: acquires the lock on an lpage.
 *      lpage_unlock: releases the lock on an lpage.
 */
void
lpage_lock(struct lpage *lp) 
{
	int spl = splhigh();
	while (LP_ISLOCKED((volatile struct lpage *) lp)) {
		thread_sleep(lp);
	}
	LP_SET(lp, LPF_LOCKED);
	splx(spl);
}

void
lpage_unlock(struct lpage *lp)
{
	int spl = splhigh();
	assert(LP_ISLOCKED(lp));
	LP_CLEAR(lp, LPF_LOCKED);
	thread_wakeup(lp);
	splx(spl);
}

/*
 * lpage_materialize: create a new lpage and allocate swap and RAM for it.
 * Mark it pinned. Do not do anything with the page contents though. 
 * Returns the lpage locked.
 */

static
int
lpage_materialize(struct lpage **lpret, paddr_t *paret)
{
	struct lpage *lp;
	paddr_t pa;
	off_t swa;

	lp = lpage_create();
	if (lp == NULL) {
		return ENOMEM;
	}

	swa = swap_alloc();
	if (swa == INVALID_SWAPADDR) {
		lpage_destroy(lp);
		return ENOSPC;
	}

	lpage_lock(lp);

	pa = coremap_allocuser(lp);
	if (pa == INVALID_PADDR) {
		swap_free(swa);
		/* Restore the reservation that swap_alloc() consumed
		 * so that a later vm_object_destroy will be able to 
		 * swap_unreserve correctly
		 */
		if (swap_reserve(1) != 0) {
			kprintf("WARNING: failed to restore swap reservation.  Expect assertion failure in swap_unreserve during vm_object_destroy.\n");
		}
		lpage_unlock(lp);
		lpage_destroy(lp);
		return ENOSPC;
	}

	lp->lp_paddr = pa | LPF_DIRTY | LPF_LOCKED;
	lp->lp_swapaddr = swa;

	assert(coremap_pageispinned(pa));

	*lpret = lp;
	*paret = pa;
	return 0;
}

/*
 * lpage_copy: create a new lpage and copy data from another lpage.
 *
 * The synchronization for this is kind of unpleasant. We do it like
 * this:
 *
 *      1. Lock oldlp.
 *      2. Check if oldlp is present.
 *      2a.    If it isn't, unlock oldlp and page in.
 *      2b.    This pins the page in the coremap.
 *      2c.    Leave the page pinned and relock oldlp.
 *      2d.    Otherwise, pin the page in the coremap.
 *      3. Now create newlp.
 *      4. Lock newlp *before* getting physical space for it.
 *         (This prevents deadlock; nobody can hold its lock.)
 *      5. Materialize a page for newlp.
 *      6. Copy.
 *      7. Unlock everything.
 *      
 */
int
lpage_copy(struct lpage *oldlp, struct lpage **lpret)
{
	struct lpage *newlp;
	paddr_t newpa, oldpa;
	off_t swa;
	int result;

	lpage_lock(oldlp);

	oldpa = oldlp->lp_paddr & PAGE_FRAME;
	if (oldpa == INVALID_PADDR) {
		swa = oldlp->lp_swapaddr;
		lpage_unlock(oldlp);

		oldpa = coremap_allocuser(oldlp);
		if (oldpa==INVALID_PADDR) {
			return ENOMEM;
		}
		assert(coremap_pageispinned(oldpa));
		lock_acquire(global_paging_lock);
		swap_pagein(oldpa, swa);
		lpage_lock(oldlp);
		lock_release(global_paging_lock);
		assert((oldlp->lp_paddr & PAGE_FRAME) == INVALID_PADDR);
		oldlp->lp_paddr = oldpa | LPF_LOCKED;
	}
	else {
		coremap_pin(oldpa);
	}
	assert(coremap_pageispinned(oldpa));

	result = lpage_materialize(&newlp, &newpa);
	if (result) {
		coremap_unpin(oldpa);
		lpage_unlock(oldlp);
		return result;
	}
	assert(coremap_pageispinned(newpa));

	assert(coremap_pageispinned(oldpa));

	coremap_copy_page(oldpa, newpa);

	assert(LP_ISDIRTY(newlp));

	coremap_unpin(newpa);
	coremap_unpin(oldpa);

	lpage_unlock(oldlp);
	lpage_unlock(newlp);

	*lpret = newlp;
	return 0;
}

/*
 * lpage_zerofill: create a new lpage and arrange for it to be cleared
 * to all zeros. The current implementation causes the lpage to be
 * resident upon return, but this is not a guaranteed property, and
 * nothing prevents the page from being evicted before it is used by
 * the caller.
 *
 * Synchronization: coremap_allocuser returns the new physical page
 * "pinned" (locked) - we hold that lock while we update the page
 * contents and the necessary lpage fields.
 */
int
lpage_zerofill(struct lpage **lpret)
{
	struct lpage *lp;
	paddr_t pa;
	int result, spl;

	result = lpage_materialize(&lp, &pa);
	if (result) {
		return result;
	}
	assert(LP_ISLOCKED(lp));
	assert(coremap_pageispinned(pa));

	coremap_zero_page(pa);

	assert(coremap_pageispinned(pa));
	coremap_unpin(pa);
	lpage_unlock(lp);

	spl = splhigh();
	ct_zerofills++;
	splx(spl);

	*lpret = lp;
	return 0;
}

/*
 * lpage_fault - handle a fault on a specific lpage. If the page is
 * not resident, get a physical page from coremap and swap it in.
 */
int
lpage_fault(struct lpage *lp, struct addrspace *as, int faulttype, vaddr_t va)
{
	// ADD CODE HERE FOR ASST2
	(void)lp;	// suppress compiler warning until code gets written
	(void)as;	// suppress compiler warning until code gets written
	(void)faulttype;// suppress compiler warning until code gets written
	(void)va;	// suppress compiler warning until code gets written
	return 1;	// suppress compiler warning until code gets written
}

/*
 * lpage_evict: Evict an lpage from physical memory.
 *
 */
void
lpage_evict(struct lpage *lp)
{
	// ADD CODE HERE FOR ASST2
	(void)lp;	// suppress compiler warning until code gets written
}
