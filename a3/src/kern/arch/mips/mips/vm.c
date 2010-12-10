#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <synch.h>
#include <vm.h>
#include <vmpvt.h>
#include <machine/coremap.h>
#include <addrspace.h>
#include <thread.h>
#include <curthread.h>
#include "opt-seqtlb.h"
#include "opt-randpage.h"

/*
 * Machine-dependent VM stuff that isn't directly coremap-related.
 */


/*
 * vm_bootstrap
 *
 * Begin VM system initialization.  Creates the coremap, which allows
 * kmalloc to be called.
 * 
 * Synchronization: none. Runs at boot.
 */
size_t
vm_bootstrap(void)
{

#if OPT_RANDPAGE
	kprintf("vm: Page replacement: random\n");
#else
	kprintf("vm: Page replacement: LRU\n");
#endif

#if OPT_SEQTLB
	kprintf("vm: TLB replacement: sequential\n");
#else
	kprintf("vm: TLB replacement: random\n");
#endif

	coremap_bootstrap();

	global_paging_lock = lock_create("global_paging_lock");

	/* Return the total size of memory */
	return mips_ramsize();
}

/*
 * vm_fault: TLB fault handler. Hands off to the current thread's
 * address space.
 *
 * Synchronization: none.
 */
int
vm_fault(int faulttype, vaddr_t faultaddress)
{
	struct addrspace *as;

	faultaddress &= PAGE_FRAME;
	assert(faultaddress < MIPS_KSEG0);

	as = curthread->t_vmspace;
	if (as == NULL) {
		return EFAULT;
	}

	return as_fault(as, faulttype, faultaddress);
}

