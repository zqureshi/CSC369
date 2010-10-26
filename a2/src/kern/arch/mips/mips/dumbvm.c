#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <thread.h>
#include <curthread.h>
#include <addrspace.h>
#include <vm.h>
#include <machine/spl.h>
#include <machine/tlb.h>

/*****************************/
/* BEGIN demke modifications */
/*****************************/

#include <bitmap.h> /* For tracking free/used physical pages */

static struct bitmap freephys_map; /* bit clear if page free, set if used */
static u_int32_t firstpage; /* first page of interest to dumbvm */
static u_int32_t lastpage;  /* last page of interest to dumbvm */

/*****************************/
/* END demke modifications   */
/*****************************/

/*
 * Dumb MIPS-only "VM system" that is intended to only be just barely
 * enough to struggle off the ground. You should replace all of this
 * code while doing the VM assignment. In fact, starting in that
 * assignment, this file is not included in your kernel!
 */

/* under dumbvm, always have 48k of user stack */
#define DUMBVM_STACKPAGES    12


void
vm_bootstrap(void)
{

	/*****************************/
	/* BEGIN demke modifications */
	/*****************************/

        unsigned int i;
	u_int32_t ramsize;
	u_int32_t min_addr; /* first address not previously allocated */
	u_int32_t max_addr; /* one PAST last usable memory address. see ram_getsize() */

	/* Get size of RAM in bytes. */
	ram_getsize(&min_addr, &max_addr);

	ramsize = max_addr; /* 0 to max_addr-1 */
	
	/* We need some memory for our bitmap of free pages... how much? */
	/* We need 1 bit to represent each page.  Divide ramsize by pagesize */
	int nbits = DIVROUNDUP(ramsize, PAGE_SIZE);

	/* We can't just call bitmap_create here, because bitmap_create
	 * uses kmalloc to allocate space for the bitmap data, and we
	 * can't use kmalloc before bootstrapping the virtual memory system.
	 * Thus, we have to hand-craft our memory allocation for the bitmap.
	 */

	/* How many bytes do we need for nbits? */
	int bytes = DIVROUNDUP(nbits, BITS_PER_WORD);

	/* How many pages are needed? */
	int npages = DIVROUNDUP(bytes, PAGE_SIZE);
	
	/* min_addr is the first unused memory address.  We'll store our
	 * bitvector at that location.
	 */
	freephys_map.nbits = nbits;
	freephys_map.v = (WORD_TYPE *)PADDR_TO_KVADDR(min_addr);

	/* The first page that dumbvm could possibly manage is the 
	 * first physical page after the bitmap that we just allocated.
	 * The lastpage is the page containing max_addr.
	 */
	firstpage = (min_addr + npages*PAGE_SIZE) / PAGE_SIZE;
	lastpage = max_addr / PAGE_SIZE;

	/* And initialize the memory bitmap */

	/* Initially, everything between firstpage and lastpage is unallocated,
	 * so set all bits to zero, and then mark bits for pages before firstpage
	 */

	bzero(freephys_map.v, bytes);

	for (i = 0; i < firstpage; i++) {
	    bitmap_mark(&freephys_map, i);
	}

	/*****************************/
	/* END demke modifications   */
	/*****************************/


	/* Do nothing. */
}

static
paddr_t
getppages(unsigned long npages)
{
	int spl;
	paddr_t addr;

	spl = splhigh();

	/*****************************/
	/* BEGIN demke modifications */
	/*****************************/
	
	u_int32_t firstpos = 0; /* index of first free page in bitmap */
	u_int32_t clearcount = 0; /* number of consecutive free pages so far */
	u_int32_t i, j; /* loop control variables */

	/* Need to find sequence of npages consecutive 0's in the bitmap. 
	 * Simple first-fit allocation.  Fragmentation is a concern that
	 * isn't dealt with.
	 */
	
	for (i = firstpage; i <= lastpage; i++) {
		if (!bitmap_isset(&freephys_map, i)) {
			clearcount++;
			if (clearcount == 1) {
				firstpos = i;
			}
			if (clearcount == npages) {
				/* We found enough free pages... mark them
				 * allocated, restore interrupts and return
				 */
				for (j = firstpos; j < firstpos+npages; j++) {
					bitmap_mark(&freephys_map, j);
				}
				splx(spl);
				return firstpos*PAGE_SIZE;
			}
		} else {
			clearcount = 0;
		}
	}

	/* If we get here, we could not allocate npages consecutive pages.
	 * Return 0 
	 */
	addr = 0;

	/*****************************/
	/* END demke modifications   */
	/*****************************/

	splx(spl);
	return addr;
}

/* Allocate/free some kernel-space virtual pages */
vaddr_t 
alloc_kpages(int npages)
{
	paddr_t pa;

	/*****************************/
	/* BEGIN demke modifications */
	/*****************************/

	if (npages > 1) {
	  kprintf("Sorry, can't allocate more than 1 kernel page at a time using dumbvm\n");
	  return 0;
	}

	/*****************************/
	/* END demke modifications   */
	/*****************************/

	pa = getppages(npages);
	if (pa==0) {
		return 0;
	}
	return PADDR_TO_KVADDR(pa);
}

void 
free_kpages(vaddr_t addr)
{

	/*****************************/
	/* BEGIN demke modifications */
	/*****************************/

	paddr_t pa = KVADDR_TO_PADDR(addr);
	u_int32_t firstpos = pa / PAGE_SIZE;

	/* We should free as many pages as were allocated in
	 * alloc_kpages, but dumbvm makes it simple by only allowing at most
	 * one page to be allocated at a time.  Thus, we just have to free
	 * the first page (the one containing addr).
	 */

	int spl = splhigh();

	bitmap_unmark(&freephys_map, firstpos);

	splx(spl);

	/*****************************/
	/* END demke modifications   */
	/*****************************/

	/* nothing */

	(void)addr;
}

int
vm_fault(int faulttype, vaddr_t faultaddress)
{
	vaddr_t vbase1, vtop1, vbase2, vtop2, stackbase, stacktop;
	paddr_t paddr;
	int i;
	u_int32_t ehi, elo;
	struct addrspace *as;
	int spl;

	spl = splhigh();

	faultaddress &= PAGE_FRAME;

	DEBUG(DB_VM, "dumbvm: fault: 0x%x\n", faultaddress);

	switch (faulttype) {
	    case VM_FAULT_READONLY:
		/* We always create pages read-write, so we can't get this */
		panic("dumbvm: got VM_FAULT_READONLY\n");
	    case VM_FAULT_READ:
	    case VM_FAULT_WRITE:
		break;
	    default:
		splx(spl);
		return EINVAL;
	}

	as = curthread->t_vmspace;
	if (as == NULL) {
		/*
		 * No address space set up. This is probably a kernel
		 * fault early in boot. Return EFAULT so as to panic
		 * instead of getting into an infinite faulting loop.
		 */
		return EFAULT;
	}

	/* Assert that the address space has been set up properly. */
	assert(as->as_vbase1 != 0);
	assert(as->as_pbase1 != 0);
	assert(as->as_npages1 != 0);
	assert(as->as_vbase2 != 0);
	assert(as->as_pbase2 != 0);
	assert(as->as_npages2 != 0);
	assert(as->as_stackpbase != 0);
	assert((as->as_vbase1 & PAGE_FRAME) == as->as_vbase1);
	assert((as->as_pbase1 & PAGE_FRAME) == as->as_pbase1);
	assert((as->as_vbase2 & PAGE_FRAME) == as->as_vbase2);
	assert((as->as_pbase2 & PAGE_FRAME) == as->as_pbase2);
	assert((as->as_stackpbase & PAGE_FRAME) == as->as_stackpbase);

	vbase1 = as->as_vbase1;
	vtop1 = vbase1 + as->as_npages1 * PAGE_SIZE;
	vbase2 = as->as_vbase2;
	vtop2 = vbase2 + as->as_npages2 * PAGE_SIZE;
	stackbase = USERSTACK - DUMBVM_STACKPAGES * PAGE_SIZE;
	stacktop = USERSTACK;

	if (faultaddress >= vbase1 && faultaddress < vtop1) {
		paddr = (faultaddress - vbase1) + as->as_pbase1;
	}
	else if (faultaddress >= vbase2 && faultaddress < vtop2) {
		paddr = (faultaddress - vbase2) + as->as_pbase2;
	}
	else if (faultaddress >= stackbase && faultaddress < stacktop) {
		paddr = (faultaddress - stackbase) + as->as_stackpbase;
	}
	else {
		splx(spl);
		return EFAULT;
	}

	/* make sure it's page-aligned */
	assert((paddr & PAGE_FRAME)==paddr);

	for (i=0; i<NUM_TLB; i++) {
		TLB_Read(&ehi, &elo, i);
		if (elo & TLBLO_VALID) {
			continue;
		}
		ehi = faultaddress;
		elo = paddr | TLBLO_DIRTY | TLBLO_VALID;
		DEBUG(DB_VM, "dumbvm: 0x%x -> 0x%x\n", faultaddress, paddr);
		TLB_Write(ehi, elo, i);
		splx(spl);
		return 0;
	}

	kprintf("dumbvm: Ran out of TLB entries - cannot handle page fault\n");
	splx(spl);
	return EFAULT;
}

struct addrspace *
as_create(void)
{
	struct addrspace *as = kmalloc(sizeof(struct addrspace));
	if (as==NULL) {
		return NULL;
	}

	as->as_vbase1 = 0;
	as->as_pbase1 = 0;
	as->as_npages1 = 0;
	as->as_vbase2 = 0;
	as->as_pbase2 = 0;
	as->as_npages2 = 0;
	as->as_stackpbase = 0;

	return as;
}

void
as_destroy(struct addrspace *as)
{

	/*****************************/
	/* BEGIN demke modifications */
	/*****************************/

  /* For each region that we allocated using as_define_region,
   * clear the bits to indicate that those pages are available again.
   */

  u_int32_t firstpos;
  u_int32_t i;

  int spl = splhigh();

  if (as->as_pbase1 != 0) {
    firstpos = as->as_pbase1 / PAGE_SIZE;
    for (i = firstpos; i < firstpos + as->as_npages1; i++)
      bitmap_unmark(&freephys_map, i);
  }

 
  if (as->as_pbase2 != 0) {
    firstpos = as->as_pbase2 / PAGE_SIZE;
    for (i = firstpos; i < firstpos + as->as_npages2; i++)
      bitmap_unmark(&freephys_map, i);
  }

  if (as->as_stackpbase != 0) {
    firstpos = as->as_stackpbase / PAGE_SIZE;
    for (i = firstpos; i < firstpos + DUMBVM_STACKPAGES; i++)
      bitmap_unmark(&freephys_map, i);
  }

  splx(spl);

	/*****************************/
	/* END demke modifications */
	/*****************************/

	kfree(as);
}

void
as_activate(struct addrspace *as)
{
	int i, spl;

	(void)as;

	spl = splhigh();

	for (i=0; i<NUM_TLB; i++) {
		TLB_Write(TLBHI_INVALID(i), TLBLO_INVALID(), i);
	}

	splx(spl);
}

int
as_define_region(struct addrspace *as, vaddr_t vaddr, size_t sz,
		 int readable, int writeable, int executable)
{
	size_t npages; 

	/* Align the region. First, the base... */
	sz += vaddr & ~(vaddr_t)PAGE_FRAME;
	vaddr &= PAGE_FRAME;

	/* ...and now the length. */
	sz = (sz + PAGE_SIZE - 1) & PAGE_FRAME;

	npages = sz / PAGE_SIZE;

	/* We don't use these - all pages are read-write */
	(void)readable;
	(void)writeable;
	(void)executable;

	if (as->as_vbase1 == 0) {
		as->as_vbase1 = vaddr;
		as->as_npages1 = npages;
		return 0;
	}

	if (as->as_vbase2 == 0) {
		as->as_vbase2 = vaddr;
		as->as_npages2 = npages;
		return 0;
	}

	/*
	 * Support for more than two regions is not available.
	 */
	kprintf("dumbvm: Warning: too many regions\n");
	return EUNIMP;
}

int
as_prepare_load(struct addrspace *as)
{
	assert(as->as_pbase1 == 0);
	assert(as->as_pbase2 == 0);
	assert(as->as_stackpbase == 0);

	as->as_pbase1 = getppages(as->as_npages1);
	if (as->as_pbase1 == 0) {
		return ENOMEM;
	}

	as->as_pbase2 = getppages(as->as_npages2);
	if (as->as_pbase2 == 0) {
		return ENOMEM;
	}

	as->as_stackpbase = getppages(DUMBVM_STACKPAGES);
	if (as->as_stackpbase == 0) {
		return ENOMEM;
	}

	return 0;
}

int
as_complete_load(struct addrspace *as)
{
	(void)as;
	return 0;
}

int
as_define_stack(struct addrspace *as, vaddr_t *stackptr)
{
	assert(as->as_stackpbase != 0);

	*stackptr = USERSTACK;
	return 0;
}

int
as_copy(struct addrspace *old, struct addrspace **ret)
{
	struct addrspace *new;

	new = as_create();
	if (new==NULL) {
		return ENOMEM;
	}

	new->as_vbase1 = old->as_vbase1;
	new->as_npages1 = old->as_npages1;
	new->as_vbase2 = old->as_vbase2;
	new->as_npages2 = old->as_npages2;

	if (as_prepare_load(new)) {
		as_destroy(new);
		return ENOMEM;
	}

	assert(new->as_pbase1 != 0);
	assert(new->as_pbase2 != 0);
	assert(new->as_stackpbase != 0);

	memmove((void *)PADDR_TO_KVADDR(new->as_pbase1),
		(const void *)PADDR_TO_KVADDR(old->as_pbase1),
		old->as_npages1*PAGE_SIZE);

	memmove((void *)PADDR_TO_KVADDR(new->as_pbase2),
		(const void *)PADDR_TO_KVADDR(old->as_pbase2),
		old->as_npages2*PAGE_SIZE);

	memmove((void *)PADDR_TO_KVADDR(new->as_stackpbase),
		(const void *)PADDR_TO_KVADDR(old->as_stackpbase),
		DUMBVM_STACKPAGES*PAGE_SIZE);
	
	*ret = new;
	return 0;
}
