#ifndef _MACHINE_SPL_H_
#define _MACHINE_SPL_H_

/*
 * Machine-independent interface to interrupt enable/disable.
 *
 * "spl" stands for "set priority level", and was originally the name of
 * a VAX assembler instruction.
 *
 * The idea is that one can block less important interrupts while
 * processing them, but still allow more urgent interrupts to interrupt
 * that processing.
 *
 * Ordinarily there would be a whole bunch of defined spl levels and
 * functions for setting them - spltty(), splbio(), etc., etc. But we
 * don't support interrupt priorities in OS/161, so there are only
 * three:
 *
 *      splhigh()    sets spl to the highest value, disabling all interrupts.
 *      spl0()       sets spl to 0, enabling all interrupts.
 *      splx(s)      sets spl to S, enabling whatever state S represents.
 *
 * All three return the old interrupt state. Thus, these are commonly used
 * as follows:
 *
 *      int s = splhigh();
 *      [ code ]
 *      splx(s);
 *
 * curspl holds the current spl level.
 *
 * in_interrupt is set to 1 if execution is presently occurring in an
 * interrupt handler. (This means that the *current* thread's normal
 * context of execution is presently stopped in the middle of doing
 * something else, which makes all kinds of things unsafe to do.)
 *
 * cpu_idle() sits around until it thinks something interesting may
 * have happened, such as an interrupt. Then it returns. It may be
 * wrong (in fact, at present, it is almost always wrong), so it
 * should be called in a loop that checks some other condition.
 * cpu_idle may be called (and in fact should be called) with
 * interrupts off - it turns them on temporarily inside itself.
 *
 * cpu_halt() stops the cpu permanently (until the external reset is
 * pushed), preferably in a low-power idle state. Looping calling
 * cpu_idle() is not good enough, because cpu_idle() accepts
 * interrupts.
 */

extern int curspl;
extern int in_interrupt;

int splhigh(void);
int spl0(void);
int splx(int);

void cpu_idle(void);
void cpu_halt(void);

/*
 * Integer spl level to use for "high".
 * This is traditionally 15. 
 */
#define SPL_HIGH   15


#endif /* _MACHINE_SPL_H_ */
