#ifndef _MIPS_SETJMP_H_
#define _MIPS_SETJMP_H_

/*
 * Must save: s0-s8, sp, ra (11 registers)
 * Don't change __JB_REGS without adjusting mips_setjmp.S accordingly.
 */
#define __JB_REGS  11

/* A jmp_buf is an array of __JB_REGS registers */
typedef u_int32_t jmp_buf[__JB_REGS];

#endif /* _MIPS_SETJMP_H_ */
