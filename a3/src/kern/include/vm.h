#ifndef _VM_H_
#define _VM_H_

#include <machine/vm.h>
#include "opt-dumbvm.h"

/*
 * VM system-related definitions.
 */

/* Fault-type arguments to vm_fault() */
#define VM_FAULT_READ        0    /* A read was attempted */
#define VM_FAULT_WRITE       1    /* A write was attempted */
#define VM_FAULT_READONLY    2    /* A write to a readonly page was attempted*/

#if OPT_DUMBVM
void vm_bootstrap(void);
#else
/* Initialization function; returns size of physical memory. */
size_t vm_bootstrap(void);
#endif

/* Initialization for swapfile; takes size returned from vm_bootstrap */
void swap_bootstrap(size_t physmemsize);

/* Shutdown function for swapfile; closes swap vnode. */
void swap_shutdown(void);

/* Print VM counters */
void vm_printstats(void);

/* Fault handling function called by trap code */
int vm_fault(int faulttype, vaddr_t faultaddress);

/* Allocate/free kernel heap pages (called by kmalloc/kfree) */
vaddr_t alloc_kpages(int npages);
void free_kpages(vaddr_t addr);

#endif /* _VM_H_ */
