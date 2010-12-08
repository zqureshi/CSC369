#ifndef _MACHINE_TLB_H_
#define _MACHINE_TLB_H_

/*
 * MIPS-specific TLB access functions.
 *
 *   TLB_Random: write the TLB entry specified by ENTRYHI and ENTRYLO
 *        into a "random" TLB slot chosen by the processor.
 *
 *        IMPORTANT NOTE: never write more than one TLB entry with the
 *        same virtual page field.
 *
 *   TLB_Write: same as TLB_Random, but you choose the slot.
 *
 *   TLB_Read: read a TLB entry out of the TLB into ENTRYHI and ENTRYLO.
 *        INDEX specifies which one to get.
 *
 *   TLB_Probe: look for an entry matching the virtual page in ENTRYHI.
 *        Returns the index, or a negative number if no matching entry
 *        was found. ENTRYLO is not actually used, but must be set; 0
 *        should be passed.
 *
 *        IMPORTANT NOTE: An entry may be matching even if the valid bit 
 *        is not set. To completely invalidate the TLB, load it with
 *        translations for addresses in one of the unmapped address
 *        ranges - these will never be matched.
 */

void TLB_Random(u_int32_t entryhi, u_int32_t entrylo);
void TLB_Write(u_int32_t entryhi, u_int32_t entrylo, u_int32_t index);
void TLB_Read(u_int32_t *entryhi, u_int32_t *entrylo, u_int32_t index);
int TLB_Probe(u_int32_t entryhi, u_int32_t entrylo);

/*
 * TLB entry fields.
 *
 * Note that the MIPS has support for a 6-bit address space ID. In the
 * interests of simplicity, we don't use it. The fields related to it
 * (TLBLO_GLOBAL and TLBHI_PID) can be left always zero, as can the
 * bits that aren't assigned a meaning.
 *
 * The TLBLO_DIRTY bit is actually a write privilege bit - it is not
 * ever set by the processor. If you set it, writes are permitted. If
 * you don't set it, you'll get a "TLB Modify" exception when a write
 * is attempted.
 *
 * There is probably no reason in the course of CS161 to use TLBLO_NOCACHE.
 */

/* Fields in the high-order word */
#define TLBHI_VPAGE   0xfffff000
/*      TLBHI_PID     0x00000fc0 */

/* Fields in the low-order word */
#define TLBLO_PPAGE   0xfffff000
#define TLBLO_NOCACHE 0x00000800
#define TLBLO_DIRTY   0x00000400
#define TLBLO_VALID   0x00000200
/*      TLBLO_GLOBAL  0x00000100 */

/*
 * Values for completely invalid TLB entries. The TLB entry index should
 * be passed to TLBHI_INVALID; this prevents loading the same invalid
 * entry into multiple TLB slots.
 */
#define TLBHI_INVALID(entryno) ((0x80000+(entryno))<<12)
#define TLBLO_INVALID()        (0)

/*
 * Number of TLB entries in the processor.
 */

#define NUM_TLB  64


#endif /* _MACHINE_TLB_H_ */
