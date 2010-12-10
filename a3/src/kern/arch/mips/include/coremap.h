/*
 * coremap - machine-dependent VM-system-internal header.
 */

#ifndef _MIPS_COREMAP_H_
#define _MIPS_COREMAP_H_

#include <machine/vm.h>		// for vaddr_t and paddr_t

struct addrspace;
struct lpage;

/*
 * Functions and definitions expected by the machine-independent VM system.
 */

/* Invalid physical address. (pa 0 is always used by the kernel.) */
#define INVALID_PADDR	((paddr_t)0)

/* MMU control */
void mmu_setas(struct addrspace *as);
void mmu_unmap(struct addrspace *as, vaddr_t va);
void mmu_map(struct addrspace *as, vaddr_t va, paddr_t pa, int writable);

/* physical page allocation */
paddr_t coremap_allocuser(struct lpage *lp);
void coremap_free(paddr_t page, int iskern);

/* physical page pinning */
void coremap_pin(paddr_t paddr);
int coremap_pageispinned(paddr_t paddr);
void coremap_unpin(paddr_t paddr);

/* special ops on physical pages */
void coremap_zero_page(paddr_t paddr);
void coremap_copy_page(paddr_t oldpaddr, paddr_t newpaddr);

/*
 * Routines for mapping physical pages into the kernel so the machine-
 * independent code can manipulate them. (This is for page content
 * manipulation other than that directly provided by the coremap
 * interface.)
 *
 * On the MIPS, because the kernel runs in direct-mapped space, all
 * physical pages are automatically visible to the kernel, so we don't
 * have to do much work. (On some architectures, these functions require
 * page table fiddling.)
 *
 * Pages should be pinned.
 */
#define coremap_map_swap_page(pa) PADDR_TO_KVADDR(pa)
#define coremap_unmap_swap_page(va, pa) {(void)(va); (void)(pa);}

/* 
 * Coremap functions whose existence is machine-dependent.
 */
void coremap_bootstrap(void);
void coremap_print_short(void);
void coremap_print_long(void);

#endif /* _MIPS_COREMAP_H_ */
