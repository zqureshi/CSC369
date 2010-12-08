#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <vm.h>
#include <vmpvt.h>
#include <addrspace.h>
#include <machine/coremap.h>
#include <machine/spl.h>
#include <machine/tlb.h>
#include <array.h>
#include <thread.h>
#include <curthread.h>
#include <vfs.h>
#include <vnode.h>
#include <kern/stat.h>
#include <bitmap.h>
#include <synch.h>
#include <uio.h>
#include <kern/unistd.h>
#include "opt-seqtlb.h"
#include "opt-randpage.h"

/*
 * MIPS coremap/MMU-control implementation.
 *
 * The MIPS has a completely software-refilled TLB. It doesn't define
 * hardware-level pagetables. Thus, for simplicity, we won't use any.
 * (In real life, one might anyway, to allow faster TLB refill
 * handling.)
 *
 * We have one coremap_entry per page of physical RAM. This is absolute
 * overhead, so it's important to keep it small - if it's overweight
 * adding more memory won't help.
 */


/*
 * The coremap allocation functions make sure that there are at least 8
 * non-kernel pages available in memory.
 */
#define CM_MIN_SLACK		8


/*
 * Coremap entry structure.
 */

struct coremap_entry {
	struct lpage *cm_lp;	/* logical page we hold, or NULL */

	int cm_tlbix:7;		/* tlb index number, or -1 */

	unsigned cm_kernel:1,	/* true if kernel page */
		cm_notlast:1,	/* true not last in sequence of kernel pages */
		cm_allocated:1;	/* true if page in use (user or kernel) */
	volatile 
	unsigned cm_pinned:1;	/* true if page is busy */

	// might want to add something to support ASST2 LRU
};

#define COREMAP_TO_PADDR(i)	(((paddr_t)PAGE_SIZE)*((i)+base_coremap_page))
#define PADDR_TO_COREMAP(page)	(((page)/PAGE_SIZE) - base_coremap_page)

////////////////////////////////////////////////////////////
//
// Variables
//

static u_int32_t num_coremap_entries;
static u_int32_t num_coremap_kernel;	/* pages allocated to the kernel */
static u_int32_t num_coremap_user;	/* pages allocated to user progs */
static u_int32_t num_coremap_free;	/* pages not allocated at all */
static u_int32_t base_coremap_page;
static struct coremap_entry *coremap;

// might want to add something to support ASST2 LRU

/* if < NUM_TLB, next TLB entry to use (when TLB not yet full) */
static u_int32_t nexttlb;

////////////////////////////////////////////////////////////
//
// TLB handling

/*
 * tlb_replace - TLB replacement algorithm. Returns index of TLB entry
 * to replace.
 *
 * Synchronization: assumes spl already high. Does not block.
 */
static
u_int32_t 
tlb_replace(void) 
{
	assert(curspl>0);

#if OPT_SEQTLB
	/* sequential */
	static u_int32_t slot = 0;
	slot = (slot + 1) % NUM_TLB;
	return slot;
#else
	/* random */
	return random() % NUM_TLB;
#endif
}

/*
 * tlb_invalidate: marks a given tlb entry as invalid.
 *
 * Synchronization: assumes spl already high. Does not block.
 */
static
void
tlb_invalidate(int tlbix)
{
	u_int32_t elo, ehi;
	paddr_t pa;
	unsigned cmix;

	assert(curspl>0);

	TLB_Read(&ehi, &elo, tlbix);
	if (elo & TLBLO_VALID) {
		pa = elo & TLBLO_PPAGE;
		cmix = PADDR_TO_COREMAP(pa);
		assert(cmix < num_coremap_entries);
		assert(coremap[cmix].cm_tlbix == tlbix);
		coremap[cmix].cm_tlbix = -1;
		DEBUG(DB_TLB, "... pa 0x%05lx --> tlb --\n", 
			(unsigned long) COREMAP_TO_PADDR(cmix));

		// might want to add something to support ASST2 LRU
	}

	TLB_Write(TLBHI_INVALID(tlbix), TLBLO_INVALID(), tlbix);
	DEBUG(DB_TLB, "... pa ------- <-- tlb %d\n", tlbix);
}

/*
 * tlb_clear: flushes the TLB by loading it with invalid entries.
 *
 * Synchronization: assumes spl already high. Does not block.
 */
static
void
tlb_clear(void)
{
	int i;	
	assert(curspl>0);
	for (i=0; i<NUM_TLB; i++) {
		tlb_invalidate(i);
	}
	nexttlb = 0;
}

/*
 * tlb_unmap: Searches the TLB for a vaddr translation and invalidates
 * it if it exists.
 *
 * Synchronization: assumes spl already high. Does not block. 
 */
static
void
tlb_unmap(vaddr_t va)
{
	int i;
	u_int32_t elo = 0, ehi = 0;

	assert(curspl > 0);

	assert(va < MIPS_KSEG0);

	i = TLB_Probe(va & PAGE_FRAME,0);
	if (i < 0) {
		return;
	}
	
	TLB_Read(&ehi, &elo, i);
	
	assert(elo & TLBLO_VALID);
	
	DEBUG(DB_TLB, "invalidating tlb slot %d (va: 0x%x)\n", i, va); 
	
	tlb_invalidate(i);
}

/*
 * mipstlb_getslot: get a TLB slot for use, replacing an existing one if
 * necessary and peforming any at-replacement actions.
 */
static
int
mipstlb_getslot(void)
{
	int i;

	if (nexttlb < NUM_TLB) {
		return nexttlb++;
	}

	/* no space... need to evict */
	i = tlb_replace();
	tlb_invalidate(i);
	return i;
}

////////////////////////////////////////////////////////////
//
// Page replacement code
//

/*
 * To evict a page, it must be non-kernel and non-pinned.
 *
 * page_replace() takes no arguments and returns an index into the
 * coremap (for the selected victim page).
 */

#if OPT_RANDPAGE

/*
 * random page replacement
 *
 */

static
u_int32_t 
page_replace(void)
{
	// ADD CODE HERE FOR ASST2
	return 0;	// dummy until code gets written
}

#else /* not OPT_RANDPAGE */

/*
 * Least-recently-used approximation, based on clock algorithm.
 *
 */

static
u_int32_t 
page_replace(void)
{
	// ADD CODE HERE FOR ASST2
	return 0;	// dummy until code gets written
}

#endif /* OPT_RANDPAGE */

////////////////////////////////////////////////////////////
//
// Setup/initialization
// 

/*
 * coremap_bootstrap
 *
 * Because of the way ram.c works, after ram_getsize() is called,
 * ram_stealmem() cannot be called any longer. This means we cannot
 * call kmalloc between calling ram_getsize and setting things up
 * properly so that kmalloc can use the coremap logic for allocation.
 *
 * What this in turn means is that we cannot use kmalloc to allocate
 * the coremap; we steal space for it ourselves by hand.
 *
 * The coremap does not manage the kernel load image itself; the kernel
 * load image is assumed to be fixed for all eternity.
 *
 * Synchronization: none; runs early in boot.
 */

void
coremap_bootstrap(void)
{
	u_int32_t i;
	paddr_t first, last;
	u_int32_t npages, coremapsize;

	nexttlb = 0;

	ram_getsize(&first, &last);

	/* The way ram.c works, these should be page-aligned */
	assert((first & PAGE_FRAME) == first);
	assert((last & PAGE_FRAME) == last);

	npages = (last - first) / PAGE_SIZE;

	DEBUG(DB_VM, "coremap: first: 0x%x, last 0x%x: %u pages\n",
	      first, last, npages);

	/*
	 * The coremap contains one coremap_entry per page.  Because
	 * of the allocation constraints here, it must be rounded up
	 * to a whole number of pages.
	 * 
	 * Note that while we don't need space for coremap entries for
	 * the coremap pages, and could save a few slots that way, the
	 * operating regime of OS/161 is such that we're unlikely to
	 * need more than one page for the coremap, and certainly not
	 * more than two. So for simplicity (and robustness) we'll
	 * avoid the relaxation computations necessary to optimize the
	 * coremap size.
	 */
	coremapsize = npages * sizeof(struct coremap_entry);
	coremapsize = ROUNDUP(coremapsize, PAGE_SIZE);
	assert((coremapsize & PAGE_FRAME) == coremapsize);

	/*
	 * Steal pages for the coremap.
	 */
	coremap = (struct coremap_entry *) PADDR_TO_KVADDR(first);
	first += coremapsize;

	if (first >= last) {
		/* This cannot happen unless coremap_entry gets really huge */
		panic("vm: coremap took up all of physical memory?\n");
	}

	/*
	 * Now, set things up to reflect the range of memory we're
	 * managing. Note that we skip the pages the coremap is using.
	 */
	base_coremap_page = first / PAGE_SIZE;
	num_coremap_entries = (last / PAGE_SIZE) - base_coremap_page;
	num_coremap_kernel = 0;
	num_coremap_user = 0;
	num_coremap_free = num_coremap_entries;

	assert(num_coremap_entries + (coremapsize/PAGE_SIZE) == npages);

	/*
	 * Initialize the coremap entries.
	 */
	for (i=0; i < num_coremap_entries; i++) {
		coremap[i].cm_kernel = 0;
		coremap[i].cm_notlast = 0;
		coremap[i].cm_allocated = 0;
		coremap[i].cm_pinned = 0;
		coremap[i].cm_tlbix = -1;
		coremap[i].cm_lp = NULL;

		// might want to add something to support ASST2 LRU
	}

	/* Suppress unused function warnings. Delete when
	 * these functions are really used somewhere.
	 */
	(void)page_replace;
}	

////////////////////////////////////////////////////////////
//
// Memory allocation
//

static
int
piggish_kernel(int proposed_kernel_pages)
{
	u_int32_t nkp;

	assert(curspl>0);

	nkp = num_coremap_kernel + proposed_kernel_pages ;
	if (nkp >= num_coremap_entries - CM_MIN_SLACK) {
		return 1;
	}
	return 0;
}

static
void
do_evict(int where)
{
	// ADD CODE HERE FOR ASST2
	(void)where;	// suppress compiler warning until code gets written
}

static
int
do_page_replace(void)
{
	int where;

	assert(curspl>0);
	assert(lock_do_i_hold(global_paging_lock));

	where = page_replace();

	assert(coremap[where].cm_pinned==0);
	assert(coremap[where].cm_kernel==0);

	if (coremap[where].cm_allocated) {
		assert(coremap[where].cm_lp!=NULL);
		assert(!in_interrupt);
		do_evict(where);
	}

	return where;
}

static
void
mark_pages_allocated(int start, int npages, int pin, int iskern)
{
	int i;
	assert(curspl>0);
	for (i=start; i<start+npages; i++) {
		assert(coremap[i].cm_pinned==0);
		assert(coremap[i].cm_allocated==0);
		assert(coremap[i].cm_kernel==0);
		assert(coremap[i].cm_lp==NULL);
		assert(coremap[i].cm_tlbix<0);

		if (pin) {
			coremap[i].cm_pinned = 1;
		}
		coremap[i].cm_allocated = 1;
		if (iskern) {
			coremap[i].cm_kernel = 1;
		}

		// might want to add something to support ASST2 LRU

		if (i < start+npages-1) {
			coremap[i].cm_notlast = 1;
		}
	}
	if (iskern) {
		num_coremap_kernel += npages;
	}
	else {
		num_coremap_user += npages;
	}
	num_coremap_free -= npages;
	assert(num_coremap_kernel+num_coremap_user+num_coremap_free
	       == num_coremap_entries);
}

/*
 * coremap_alloc_one_page
 *
 * Allocate one page of memory, mark it pinned if requested, and
 * return its paddr. The page is marked a kernel page iff the lp
 * argument is NULL.
 */
static
paddr_t
coremap_alloc_one_page(struct lpage *lp, int dopin)
{
	int spl, candidate, i, iskern;

	iskern = (lp == NULL);

	/*
	 * Hold this while allocating to reduce starvation of multipage
	 * allocations. (But we can't if we're in an interrupt, or if
	 * we're still very early in boot.)
	 */
	if (!in_interrupt && curthread!=NULL) {
		lock_acquire(global_paging_lock);
	}

	spl = splhigh();

	/*
	 * Don't allow the kernel to eat everything.
	 */
	if (iskern && piggish_kernel(1)) {
		coremap_print_short();
		splx(spl);
		if (!in_interrupt && curthread!=NULL) {
			lock_release(global_paging_lock);
		}
		kprintf("alloc_kpages: kernel heap full getting 1 page\n");
		return INVALID_PADDR;
	}

	/*
	 * For single-page allocations, start at the top end of memory. We
	 * will do multi-page allocations at the bottom end in the hope of
	 * reducing long-term fragmentation. But it probably won't help
	 * much if the system gets busy.
	 */

	candidate = -1;

	if (num_coremap_free > 0) {
		/* There's a free page. Find it. */

		for (i = num_coremap_entries-1; i>=0; i--) {
			if (coremap[i].cm_pinned || coremap[i].cm_allocated) {
				continue;
			}
			assert(coremap[i].cm_kernel==0);
			assert(coremap[i].cm_lp==NULL);
			candidate = i;
			break;
		}
	}

	if (candidate < 0 && !in_interrupt) {
		assert(num_coremap_free==0);
		candidate = do_page_replace();
	}

	if (candidate < 0) {
		splx(spl);
		/* release global_paging_lock before returning */
		if (!in_interrupt && curthread!=NULL) {
			lock_release(global_paging_lock);
		}
		return INVALID_PADDR;
	}

	/* At this point we should have an ok page. */
	mark_pages_allocated(candidate, 1 /* npages */, dopin, iskern);
	coremap[candidate].cm_lp = lp;

	// free pages should not be in the TLB
	assert(coremap[candidate].cm_tlbix<0);

	splx(spl);
	if (!in_interrupt && curthread!=NULL) {
		lock_release(global_paging_lock);
	}

	return COREMAP_TO_PADDR(candidate);
}

static
paddr_t
coremap_alloc_multipages(unsigned npages)
{
	int base, bestbase;
	int badness, bestbadness;
	int evicted;
	int spl;
	unsigned i;

	assert(npages>1);

	/*
	 * Get this early and hold it during the allocation so nobody else
	 * can start paging while we're trying to page out the victims in
	 * the allocation range.
	 */

	if (!in_interrupt && curthread!=NULL) {
		lock_acquire(global_paging_lock);
	}

	spl = splhigh();

	if (piggish_kernel(npages)) {
		coremap_print_short();
		splx(spl);
		if (!in_interrupt && curthread!=NULL) {
			lock_release(global_paging_lock);
		}
		kprintf("alloc_kpages: kernel heap full getting %u pages\n",
			npages);
		return INVALID_PADDR;
	}

	/*
	 * Look for the best block of this length.
	 * "badness" counts how many evictions we need to do.
	 * Find the block where it's smallest.
	 */

	do {
		bestbase = -1;
		bestbadness = npages*2;
		base = -1;
		badness = 0;
		for (i=0; i<num_coremap_entries; i++) {
			if (coremap[i].cm_pinned || coremap[i].cm_kernel) {
				base = -1;
				badness = 0;
				continue;
			}
			
			if (coremap[i].cm_allocated) {
				assert(coremap[i].cm_lp != NULL);
				/*
				 * We should do badness += 2 if page
				 * needs cleaning, but we don't know
				 * that here for now. Also, we shouldn't
				 * prefer clean pages when there isn't a
				 * pageout thread, as we'll end up always
				 * replacing code and never data, which
				 * doesn't work well. FUTURE.
				 */
				badness++;
			}
			
			if (base < 0) {
				base = i;
			}
			else if (i - base >= npages-1) {
				if (badness < bestbadness) {
					bestbase = base;
					bestbadness = badness;
				}

				/* Keep trying (offset upwards by one) */
				if (coremap[base].cm_allocated) {
					badness--;
				}
				base++;
			}
		}

		if (bestbase < 0) {
			/* no good */
			splx(spl);
			if (!in_interrupt && curthread!=NULL) {
				lock_release(global_paging_lock);
			}
			return INVALID_PADDR;
		}

		/*
		 * If any pages need evicting, evict them and try the
		 * whole schmear again. Because we are holding
		 * global_paging_lock, nobody else *ought* to allocate
		 * or pin these pages until we're done. But the
		 * contract with global_paging_lock is that it's
		 * advisory -- so tolerate and retry if/in case
		 * something changes while we're paging.
		 */

		evicted = 0;
		for (i=bestbase; i<bestbase+npages; i++) {
			if (coremap[i].cm_pinned || coremap[i].cm_kernel) {
				/* Whoops... retry */
				assert(evicted==1);
				break;
			}
			if (coremap[i].cm_allocated) {
				if (in_interrupt) {
					/* Can't evict here */
					splx(spl);
					/* don't need to unlock */
					return INVALID_PADDR;
				}
				do_evict(i);
				evicted = 1;
			}
		}
	} while (evicted);

	mark_pages_allocated(bestbase, npages, 
			     0 /* pinned -- unnecessary */,
			     1 /* kernel */);
				     
	splx(spl);
	if (!in_interrupt && curthread!=NULL) {
		lock_release(global_paging_lock);
	}
	return COREMAP_TO_PADDR(bestbase);
}

/*
 * coremap_allocuser
 *
 * Allocate a page for a user-level process, to hold the passed-in
 * logical page.
 *
 * Synchronization: sets splhigh. 
 * May block to swap pages out.
 */
paddr_t
coremap_allocuser(struct lpage *lp)
{
	assert(!in_interrupt);
	return coremap_alloc_one_page(lp, 1 /* dopin */);
}

/*
 * coremap_free 
 *
 * Deallocates the passed paddr and any subsequent pages allocated in
 * the same block. Cross-checks the iskern flag against the flags
 * maintained in the coremap entry.
 *
 * Synchronization: sets splhigh. Does not block.
 */
void
coremap_free(paddr_t page, int iskern)
{
	u_int32_t i, ppn;
	int spl;

	ppn = PADDR_TO_COREMAP(page);	
	
	spl = splhigh();

	assert(ppn<num_coremap_entries);

	for (i = ppn; i < num_coremap_entries; i++) {
		if (!coremap[i].cm_allocated) {
			panic("coremap_free: freeing free page (pa 0x%x)\n",
			      COREMAP_TO_PADDR(i));
		}

		// For the time being, the page will actually be pinned,
		// due to synchronization difficulties. See lpage.c. This
		// is not clean and really ought to be changed around.
		//
		//if (coremap[i].cm_pinned) {
		//	panic("coremap_free: freeing pinned page (pa 0x%x)\n",
		//	      COREMAP_TO_PADDR(i));
		//}

		/* flush any live mapping */
		if (coremap[i].cm_tlbix >= 0) {
			tlb_invalidate(coremap[i].cm_tlbix);
			coremap[i].cm_tlbix = -1;
			DEBUG(DB_TLB, "... pa 0x%05lx --> tlb --\n", 
				(unsigned long) COREMAP_TO_PADDR(i));
		}

		DEBUG(DB_VM,"coremap_free: freeing pa 0x%x\n",
		      COREMAP_TO_PADDR(i));

		/* now we can actually deallocate the page */

		coremap[i].cm_allocated = 0;
		if (coremap[i].cm_kernel) {
			assert(coremap[i].cm_lp == NULL);
			num_coremap_kernel--;
			assert(iskern);
			coremap[i].cm_kernel = 0;
		}
		else {
			assert(coremap[i].cm_lp != NULL);
			num_coremap_user--;
			assert(!iskern);
		}
		num_coremap_free++;

		// might want to add something to support ASST2 LRU

		coremap[i].cm_lp = NULL;

		if (!coremap[i].cm_notlast) {
			break;
		}

		coremap[i].cm_notlast = 0;
	}

	splx(spl);
}

/*
 * alloc_kpages
 *
 * Allocate some kernel-space virtual pages.
 * This is the interface kmalloc uses to get pages for its use.
 *
 * Synchronization: sets splhigh.
 * May block to swap pages out.
 */
vaddr_t 
alloc_kpages(int npages)
{
	paddr_t pa;
	if (npages > 1) {
		pa = coremap_alloc_multipages(npages);
	}
	else {
		pa = coremap_alloc_one_page(NULL, 0 /* dopin */);
	}
	if (pa==INVALID_PADDR) {
		return 0;
	}
	return PADDR_TO_KVADDR(pa);
}

/*
 * free_kpages
 *
 * Free pages allocated with alloc_kpages.
 * Synchronization: sets splhigh. Does not block.
 */
void 
free_kpages(vaddr_t addr)
{
	coremap_free(KVADDR_TO_PADDR(addr), 1 /* iskern */);
}

////////////////////////////////////////////////////////////

/*
 * coremap_print_short: diagnostic dump of coremap to console.
 *
 * synchronization: sets splhigh. Does not block.
 */
#define NCOLS 64
void					
coremap_print_short(void)
{
	u_int32_t i, atbol=1;
	int spl;

	spl = splhigh();
		
	kprintf("Coremap: %u entries, %uk/%uu/%uf\n",
		num_coremap_entries,
		num_coremap_kernel, num_coremap_user, num_coremap_free);

	for (i=0; i<num_coremap_entries; i++) {
		if (atbol) {
			kprintf("0x%x: ", COREMAP_TO_PADDR(i));
			atbol=0;
		}
		if (coremap[i].cm_kernel && coremap[i].cm_notlast) {
			kprintf("k");
		}
		else if (coremap[i].cm_kernel) {
			kprintf("K");
		}
		else if (coremap[i].cm_allocated && coremap[i].cm_pinned) {
			kprintf("&");
		}
		else if (coremap[i].cm_allocated) {
			kprintf("*");
		}
		else {
			kprintf(".");
		}
		if (i%NCOLS==NCOLS-1) {
			kprintf("\n");
			atbol=1;
		}
	}
	if (!atbol) {
		kprintf("\n");
	}
	splx(spl);
}
#undef NCOLS

/*
 * coremap_print_long: debugging dump of coremap to console.
 * 
 * synchronization: sets splhigh. Does not block.
 */
#if 0 /* not used, but potentially useful */
#define NCOLS 5
void					
coremap_print_long(void)
{
	u_int32_t i, j, colsize, cn;
	int spl;

	spl = splhigh();
		
	kprintf("Coremap: first: %u, %u entries, %uk/%uu/%uf\n",
		base_coremap_page, num_coremap_entries,
		num_coremap_kernel, num_coremap_user, num_coremap_free);

	colsize = DIVROUNDUP(num_coremap_entries, NCOLS);
	for (i=0; i<colsize; i++) {
		for (j=0; j<NCOLS; j++) {
			cn = j*colsize + i;
			if (cn >= num_coremap_entries) {
				continue;
			}

			kprintf("0x%x:", COREMAP_TO_PADDR(cn));
			if (coremap[cn].cm_kernel) {
				assert(coremap[cn].cm_lp == NULL);
				kprintf("K%s\t", 
					coremap[cn].cm_notlast ? "S" : "-");
			}
			else if (coremap[cn].cm_allocated) {
				assert(coremap[cn].cm_lp != NULL);
				assert(coremap[cn].cm_notlast==0);
				kprintf("0x%lx\t",
					(long) coremap[cn].cm_lp->lp_swapaddr);
			}
			else {
				assert(coremap[cn].cm_lp == NULL);
				assert(coremap[cn].cm_notlast==0);
				kprintf("free\t");
			}
		}
		kprintf("\n");
	}
	kprintf("\n");
	splx(spl);
}
#undef NCOLS
#endif /* 0 */

/*
 * coremap_pin: mark page pinned for manipulation of contents.
 *
 * Synchronization: sets splhigh. Blocks if page is already pinned.
 */
void
coremap_pin(paddr_t paddr)
{
	int spl;
	unsigned ix;
	ix = PADDR_TO_COREMAP(paddr);
	assert(ix<num_coremap_entries);

	spl = splhigh();
	while (coremap[ix].cm_pinned) {
		thread_sleep(&coremap[ix]);
	}
	coremap[ix].cm_pinned = 1;
	splx(spl);
}

/*
 * coremap_pageispinned: checks if page is marked pinned.
 */
int
coremap_pageispinned(paddr_t paddr)
{
	int spl, rv;
	unsigned ix;

	ix = PADDR_TO_COREMAP(paddr);
	assert(ix<num_coremap_entries);

	spl = splhigh();
	rv = coremap[ix].cm_pinned != 0;
	splx(spl);

	return rv;
}

/*
 * coremap_unpin: unpin a page that was pinned with coremap_pin or
 * coremap_allocuser.
 *
 * Synchronization: sets splhigh. Does not block.
 */
void
coremap_unpin(paddr_t paddr)
{
	int spl;
	unsigned ix;

	ix = PADDR_TO_COREMAP(paddr);
	assert(ix<num_coremap_entries);

	spl = splhigh();
	assert(coremap[ix].cm_pinned);
	coremap[ix].cm_pinned = 0;
	thread_wakeup(&coremap[ix]);
	splx(spl);
}

/*
 * coremap_zero_page: zero out a memory page. Page should be pinned.
 *
 * Synchronization: none. Does not block.
 */

void
coremap_zero_page(paddr_t paddr)
{
	vaddr_t va;

	assert(coremap_pageispinned(paddr));

	va = PADDR_TO_KVADDR(paddr);
	bzero((char *)va, PAGE_SIZE);
}

/*
 * coremap_copy_page: copy a memory page. Both pages should be pinned.
 *
 * Synchronization: none. Does not block.
 */

void
coremap_copy_page(paddr_t oldpaddr, paddr_t newpaddr)
{
	vaddr_t oldva, newva;

	assert(oldpaddr != newpaddr);
	assert(coremap_pageispinned(oldpaddr));
	assert(coremap_pageispinned(newpaddr));

	oldva = PADDR_TO_KVADDR(oldpaddr);
	newva = PADDR_TO_KVADDR(newpaddr);
	memcpy((char *)newva, (char *)oldva, PAGE_SIZE);
}

////////////////////////////////////////////////////////////

/*
 * Hardware page-table interface
 */

static struct addrspace *lastas = NULL;

/*
 * mmu_setas: Set current address space in MMU.
 *
 * Synchronization: sets spl high. Does not block.
 */
void
mmu_setas(struct addrspace *as)
{
	int spl;

	spl = splhigh();
	if (as != lastas) {
		lastas = as;
		tlb_clear();
	}
	splx(spl);
}

/*
 * mmu_unmap: Remove a translation from the MMU.
 *
 * Synchronization: sets splhigh. Does not block.
 */
void
mmu_unmap(struct addrspace *as, vaddr_t va)
{
	int spl;

	spl = splhigh();
	if (as == lastas) {
		tlb_unmap(va);
	}
	splx(spl);
}

/*
 * mmu_map: Enter a translation into the MMU. (This is the end result
 * of fault handling.)
 *
 * Synchronization: sets splhigh. Does not block.
 */
void
mmu_map(struct addrspace *as, vaddr_t va, paddr_t pa, int writable)
{
	int tlbix, spl;
	u_int32_t ehi, elo;
	unsigned cmix;

	assert(as==lastas);

	assert(pa/PAGE_SIZE >= base_coremap_page);
	assert(pa/PAGE_SIZE - base_coremap_page < num_coremap_entries);
	
	spl = splhigh();

	tlbix = TLB_Probe(va, 0);
	if (tlbix < 0) {
		tlbix = mipstlb_getslot();
	}
	assert(tlbix>=0 && tlbix<NUM_TLB);

	cmix = PADDR_TO_COREMAP(pa);
	assert(cmix < num_coremap_entries);
	if (coremap[cmix].cm_tlbix != tlbix) {
		assert(coremap[cmix].cm_tlbix == -1);
		coremap[cmix].cm_tlbix = tlbix;
		DEBUG(DB_TLB, "... pa 0x%05lx <-> tlb %d\n", 
			(unsigned long) COREMAP_TO_PADDR(cmix), tlbix);
	}

	ehi = va & TLBHI_VPAGE;
	elo = (pa & TLBLO_PPAGE) | TLBLO_VALID;
	if (writable) {
		elo |= TLBLO_DIRTY;
	}

	TLB_Write(ehi, elo, tlbix);

	// might want to add something to support ASST2 LRU

	splx(spl);
}
