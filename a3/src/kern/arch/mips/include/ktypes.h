#ifndef _MIPS_KTYPES_H_
#define _MIPS_KTYPES_H_

/*
 * Some other machine-dependent types. These should not be visible to
 * userland and thus can't go in <machine/types.h>.
 */

typedef u_int32_t paddr_t;   /* type for a physical address */
typedef u_int32_t vaddr_t;   /* type for a virtual address */

#endif /* _MIPS_KTYPES_H_ */
