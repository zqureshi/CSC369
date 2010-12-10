#ifndef _MIPS_SWITCHFRAME_H_
#define _MIPS_SWITCHFRAME_H_

/*
 * Structure describing what is saved on the stack during a context switch.
 *
 * This must agree with the code in switch.S.
 */

struct switchframe {
	u_int32_t sf_s0;
	u_int32_t sf_s1;
	u_int32_t sf_s2;
	u_int32_t sf_s3;
	u_int32_t sf_s4;
	u_int32_t sf_s5;
	u_int32_t sf_s6;
	u_int32_t sf_s7;
	u_int32_t sf_s8;
	u_int32_t sf_gp;
	u_int32_t sf_ra;
};

#endif /* _MIPS_SWITCHFRAME_H_ */
