#include <types.h>
#include <lib.h>
#include <vm.h>
#include <machine/spl.h>

#include "opt-dumbvm.h"  /* demke: Warn about large allocations if DUMB_VM */

static
void
fill_deadbeef(void *vptr, size_t len)
{
	u_int32_t *ptr = vptr;
	size_t i;

	for (i=0; i<len/sizeof(u_int32_t); i++) {
		ptr[i] = 0xdeadbeef;
	}
}

////////////////////////////////////////////////////////////
//
// Pool-based subpage allocator.
//
// It works like this:
//
//    We allocate one page at a time and fill it with objects of size k,
//    for various k. Each page has its own freelist, maintained by a
//    linked list in the first word of each object. Each page also has a
//    freecount, so we know when the page is completely free and can 
//    release it.
//
//    No assumptions are made about the sizes k; they need not be
//    powers of two. Note, however, that malloc must always return
//    pointers aligned to the maximum alignment requirements of the
//    platform; thus block sizes must at least be multiples of 4,
//    preferably 8. They must also be at least sizeof(struct
//    freelist). It is only worth defining an additional block size if
//    more blocks would fit on a page than with the existing block
//    sizes, and large numbers of items of the new size are allocated.
//
//    The free counts and addresses of the pages are maintained in
//    another list.  Maintaining this table is a nuisance, because it
//    cannot recursively use the subpage allocator. (We could probably
//    make that work, but it would be painful.)
//

#undef  SLOW	/* consistency checks */
#undef SLOWER	/* lots of consistency checks */

////////////////////////////////////////

#if PAGE_SIZE == 4096

#define NSIZES 8
static const size_t sizes[NSIZES] = { 16, 32, 64, 128, 256, 512, 1024, 2048 };

#define SMALLEST_SUBPAGE_SIZE 16
#define LARGEST_SUBPAGE_SIZE 2048

#elif PAGE_SIZE == 8192
#error "No support for 8k pages"
#else
#error "Odd page size"
#endif

////////////////////////////////////////

struct freelist {
	struct freelist *next;
};

struct pageref {
	struct pageref *next_samesize;
	struct pageref *next_all;
	vaddr_t pageaddr_and_blocktype;
	u_int16_t freelist_offset;
	u_int16_t nfree;
};

#define INVALID_OFFSET   (0xffff)

#define PR_PAGEADDR(pr)  ((pr)->pageaddr_and_blocktype & PAGE_FRAME)
#define PR_BLOCKTYPE(pr) ((pr)->pageaddr_and_blocktype & ~PAGE_FRAME)
#define MKPAB(pa, blk)   (((pa)&PAGE_FRAME) | ((blk) & ~PAGE_FRAME))

////////////////////////////////////////

/*
 * This is cheesy. 
 *
 * The problem is not that it's wasteful - we can only allocate whole
 * pages of pageref structures at a time anyway. The problem is that
 * we really ought to be able to have more than one of these pages.
 *
 * However, for the time being, one page worth of pagerefs gives us
 * 256 pagerefs; this lets us manage 256 * 4k = 1M of kernel heap.
 * That would be twice as much memory as we get for *everything*.
 * Thus, we will cheat and not allow any mechanism for having a second
 * page of pageref structs.
 *
 * Then, since the size is fixed at one page, we'll simplify a bit
 * further by allocating the page in the kernel BSS instead of calling
 * alloc_kpages to get it.
 */

#define NPAGEREFS (PAGE_SIZE / sizeof(struct pageref))
static struct pageref pagerefs[NPAGEREFS];

#define INUSE_WORDS (NPAGEREFS/32)
static u_int32_t pagerefs_inuse[INUSE_WORDS];

static
struct pageref *
allocpageref(void)
{
	unsigned i,j;
	u_int32_t k;

	for (i=0; i<INUSE_WORDS; i++) {
		if (pagerefs_inuse[i]==0xffffffff) {
			/* full */
			continue;
		}
		for (k=1,j=0; k!=0; k<<=1,j++) {
			if ((pagerefs_inuse[i] & k)==0) {
				pagerefs_inuse[i] |= k;
				return &pagerefs[i*32 + j];
			}
		}
		assert(0);
	}

	/* ran out */
	return NULL;
}

static
void
freepageref(struct pageref *p)
{
	size_t i, j;
	u_int32_t k;

	j = p-pagerefs;
	assert(j < NPAGEREFS);  /* note: j is unsigned, don't test < 0 */
	i = j/32;
	k = ((u_int32_t)1) << (j%32);
	assert((pagerefs_inuse[i] & k) != 0);
	pagerefs_inuse[i] &= ~k;
}

////////////////////////////////////////

static struct pageref *sizebases[NSIZES];
static struct pageref *allbase;

////////////////////////////////////////

/* SLOWER implies SLOW */
#ifdef SLOWER
#ifndef SLOW
#define SLOW
#endif
#endif

#ifdef SLOW
static
void
checksubpage(struct pageref *pr)
{
	vaddr_t prpage, fla;
	struct freelist *fl;
	int blktype;
	int nfree=0;

	assert(curspl>0);

	if (pr->freelist_offset == INVALID_OFFSET) {
		assert(pr->nfree==0);
		return;
	}

	prpage = PR_PAGEADDR(pr);
	blktype = PR_BLOCKTYPE(pr);

	assert(pr->freelist_offset < PAGE_SIZE);
	assert(pr->freelist_offset % sizes[blktype] == 0);

	fla = prpage + pr->freelist_offset;
	fl = (struct freelist *)fla;

	for (; fl != NULL; fl = fl->next) {
		fla = (vaddr_t)fl;
		assert(fla >= prpage && fla < prpage + PAGE_SIZE);
		assert((fla-prpage) % sizes[blktype] == 0);
		assert(fla >= MIPS_KSEG0);
		assert(fla < MIPS_KSEG1);
		nfree++;
	}
	assert(nfree==pr->nfree);
}
#else
#define checksubpage(pr) ((void)(pr))
#endif

#ifdef SLOWER
static
void
checksubpages(void)
{
	struct pageref *pr;
	int i;
	unsigned sc=0, ac=0;

	assert(curspl>0);

	for (i=0; i<NSIZES; i++) {
		for (pr = sizebases[i]; pr != NULL; pr = pr->next_samesize) {
			checksubpage(pr);
			assert(sc < NPAGEREFS);
			sc++;
		}
	}

	for (pr = allbase; pr != NULL; pr = pr->next_all) {
		checksubpage(pr);
		assert(ac < NPAGEREFS);
		ac++;
	}

	assert(sc==ac);
}
#else
#define checksubpages() 
#endif

////////////////////////////////////////

static
void
dumpsubpage(struct pageref *pr)
{
	vaddr_t prpage, fla;
	struct freelist *fl;
	int blktype;
	unsigned i, n, index;
	u_int32_t freemap[PAGE_SIZE / (SMALLEST_SUBPAGE_SIZE*32)];

	checksubpage(pr);
	assert(curspl>0);

	/* clear freemap[] */
	for (i=0; i<sizeof(freemap)/sizeof(freemap[0]); i++) {
		freemap[i] = 0;
	}

	prpage = PR_PAGEADDR(pr);
	blktype = PR_BLOCKTYPE(pr);

	/* compute how many bits we need in freemap and assert we fit */
	n = PAGE_SIZE / sizes[blktype];
	assert(n <= 32*sizeof(freemap)/sizeof(freemap[0]));

	if (pr->freelist_offset != INVALID_OFFSET) {
		fla = prpage + pr->freelist_offset;
		fl = (struct freelist *)fla;

		for (; fl != NULL; fl = fl->next) {
			fla = (vaddr_t)fl;
			index = (fla-prpage) / sizes[blktype];
			assert(index<n);
			freemap[index/32] |= (1<<(index%32));
		}
	}

	kprintf("at 0x%08lx: size %-4lu  %u/%u free\n", 
		(unsigned long)prpage, (unsigned long) sizes[blktype],
		(unsigned) pr->nfree, n);
	kprintf("   ");
	for (i=0; i<n; i++) {
		int val = (freemap[i/32] & (1<<(i%32)))!=0;
		kprintf("%c", val ? '.' : '*');
		if (i%64==63 && i<n-1) {
			kprintf("\n   ");
		}
	}
	kprintf("\n");
}

void
kheap_printstats(void)
{
	struct pageref *pr;

	/* print the whole thing with interrupts off */
	int spl = splhigh();

	kprintf("Subpage allocator status:\n");

	for (pr = allbase; pr != NULL; pr = pr->next_all) {
		dumpsubpage(pr);
	}

	splx(spl);
}

////////////////////////////////////////

static
void
remove_lists(struct pageref *pr, int blktype)
{
	struct pageref **guy;

	assert(blktype>=0 && blktype<NSIZES);

	for (guy = &sizebases[blktype]; *guy; guy = &(*guy)->next_samesize) {
		checksubpage(*guy);
		if (*guy == pr) {
			*guy = pr->next_samesize;
			break;
		}
	}

	for (guy = &allbase; *guy; guy = &(*guy)->next_all) {
		checksubpage(*guy);
		if (*guy == pr) {
			*guy = pr->next_all;
			break;
		}
	}
}

static
inline
int blocktype(size_t sz)
{
	unsigned i;
	for (i=0; i<NSIZES; i++) {
		if (sz <= sizes[i]) {
			return i;
		}
	}

	panic("Subpage allocator cannot handle allocation of size %lu\n", 
	      (unsigned long)sz);

	// keep compiler happy
	return 0;
}

static
void *
subpage_kmalloc(size_t sz)
{
	int spl;		// saved interrupt level
	unsigned blktype;	// index into sizes[] that we're using
	struct pageref *pr;	// pageref for page we're allocating from
	vaddr_t prpage;		// PR_PAGEADDR(pr)
	vaddr_t fla;		// free list entry address
	struct freelist *volatile fl;	// free list entry
	void *retptr;		// our result

	volatile int i;


	blktype = blocktype(sz);
	sz = sizes[blktype];

	spl = splhigh();

	checksubpages();

	for (pr = sizebases[blktype]; pr != NULL; pr = pr->next_samesize) {

		/* check for corruption */
		assert(PR_BLOCKTYPE(pr) == blktype);
		checksubpage(pr);

		if (pr->nfree > 0) {

		doalloc: /* comes here after getting a whole fresh page */

			assert(pr->freelist_offset < PAGE_SIZE);
			prpage = PR_PAGEADDR(pr);
			fla = prpage + pr->freelist_offset;
			fl = (struct freelist *)fla;

			retptr = fl;
			fl = fl->next;
			pr->nfree--;

			if (fl != NULL) {
				assert(pr->nfree > 0);
				fla = (vaddr_t)fl;
				assert(fla - prpage < PAGE_SIZE);
				pr->freelist_offset = fla - prpage;
			}
			else {
				assert(pr->nfree == 0);
				pr->freelist_offset = INVALID_OFFSET;
			}

			checksubpages();

			splx(spl);
			return retptr;
		}
	}

	/*
	 * No page of the right size available.
	 * Make a new one.
	 */

	pr = allocpageref();
	if (pr==NULL) {
		/* Couldn't allocate accounting space for the new page. */
		splx(spl);
		kprintf("kmalloc: Subpage allocator couldn't get pageref\n"); 
		return NULL;
	}

	prpage = alloc_kpages(1);
	if (prpage==0) {
		/* Out of memory. */
		freepageref(pr);
		splx(spl);
		kprintf("kmalloc: Subpage allocator couldn't get a page\n"); 
		return NULL;
	}

	pr->pageaddr_and_blocktype = MKPAB(prpage, blktype);
	pr->nfree = PAGE_SIZE / sizes[blktype];

	/*
	 * Note: fl is volatile because the MIPS toolchain we were
	 * using in spring 2001 attempted to optimize this loop and
	 * blew it. Making fl volatile inhibits the optimization.
	 */

	fla = prpage;
	fl = (struct freelist *)fla;
	fl->next = NULL;
	for (i=1; i<pr->nfree; i++) {
		fl = (struct freelist *)(fla + i*sizes[blktype]);
		fl->next = (struct freelist *)(fla + (i-1)*sizes[blktype]);
		assert(fl != fl->next);
	}
	fla = (vaddr_t) fl;
	pr->freelist_offset = fla - prpage;
	assert(pr->freelist_offset == (pr->nfree-1)*sizes[blktype]);

	pr->next_samesize = sizebases[blktype];
	sizebases[blktype] = pr;

	pr->next_all = allbase;
	allbase = pr;

	/* This is kind of cheesy, but avoids duplicating the alloc code. */
	goto doalloc;
}

static
int
subpage_kfree(void *ptr)
{
	int spl;		// saved interrupt level
	int blktype;		// index into sizes[] that we're using
	vaddr_t ptraddr;	// same as ptr
	struct pageref *pr;	// pageref for page we're freeing in
	vaddr_t prpage;		// PR_PAGEADDR(pr)
	vaddr_t fla;		// free list entry address
	struct freelist *fl;	// free list entry
	vaddr_t offset;		// offset into page

	ptraddr = (vaddr_t)ptr;

	spl = splhigh();

	checksubpages();

	for (pr = allbase; pr; pr = pr->next_all) {
		prpage = PR_PAGEADDR(pr);
		blktype = PR_BLOCKTYPE(pr);

		/* check for corruption */
		assert(blktype>=0 && blktype<NSIZES);
		checksubpage(pr);

		if (ptraddr >= prpage && ptraddr < prpage + PAGE_SIZE) {
			break;
		}
	}

	if (pr==NULL) {
		/* Not on any of our pages - not a subpage allocation */
		splx(spl);
		return -1;
	}

	offset = ptraddr - prpage;

	/* Check for proper positioning and alignment */
	if (offset >= PAGE_SIZE || offset % sizes[blktype] != 0) {
		panic("kfree: subpage free of invalid addr %p\n", ptr);
	}

	/*
	 * Clear the block to 0xdeadbeef to make it easier to detect
	 * uses of dangling pointers.
	 */
	fill_deadbeef(ptr, sizes[blktype]);

	/*
	 * We probably ought to check for free twice by seeing if the block
	 * is already on the free list. But that's expensive, so we don't.
	 */

	fla = prpage + offset;
	fl = (struct freelist *)fla;
	if (pr->freelist_offset == INVALID_OFFSET) {
		fl->next = NULL;
	} else {
		fl->next = (struct freelist *)(prpage + pr->freelist_offset);
	}
	pr->freelist_offset = offset;
	pr->nfree++;

	assert(pr->nfree <= PAGE_SIZE / sizes[blktype]);
	if (pr->nfree == PAGE_SIZE / sizes[blktype]) {
		/* Whole page is free. */
		remove_lists(pr, blktype);
		free_kpages(prpage);
		freepageref(pr);
	}

	checksubpages();

	splx(spl);
	return 0;
}

//
////////////////////////////////////////////////////////////

void *
kmalloc(size_t sz)
{
	if (sz>=LARGEST_SUBPAGE_SIZE) {
		unsigned long npages;
		vaddr_t address;

		/* Round up to a whole number of pages. */
		npages = (sz + PAGE_SIZE - 1)/PAGE_SIZE;
#if OPT_DUMBVM
		/* demke: dumbvm can't free more than 1 page at a time
		 * so it really isn't a good idea to allocate more than
		 * one at a time.  Unless you are pretty sure you never
		 * want to free those pages.
		 */
		if (npages > 1) {
		        kprintf("kmalloc: WARNING - not allowed to allocate more than 1 page using DUMBVM\n");
			return NULL;
		}
#endif
		address = alloc_kpages(npages);
		if (address==0) {
			return NULL;
		}

		return (void *)address;
	}

	return subpage_kmalloc(sz);
}

void
kfree(void *ptr)
{
	/*
	 * Try subpage first; if that fails, assume it's a big allocation.
	 */
	if (ptr == NULL) {
		return;
	} else if (subpage_kfree(ptr)) {
		assert((vaddr_t)ptr%PAGE_SIZE==0);
		free_kpages((vaddr_t)ptr);
	}
}

