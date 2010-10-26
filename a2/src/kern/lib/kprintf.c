#include <types.h>
#include <stdarg.h>
#include <kern/unistd.h>
#include <lib.h>
#include <synch.h>
#include <vfs.h>          // for vfs_sync()
#include <thread.h>       // for thread_panic();
#include <machine/pcb.h>  // for md_panic()
#include <machine/spl.h>

/* Flags word for DEBUG() macro. */
u_int32_t dbflags = 0;

/* Lock for non-polled kprintfs */
static struct lock *kprintf_lock;

/*
 * Warning: all this has to work from interrupt handlers and when
 * interrupts are disabled.
 */


/* Send characters to the console. */
static
void
console_send(void *junk, const char *data, size_t len)
{
	size_t i;

	(void)junk;

	for (i=0; i<len; i++) {
		putch(data[i]);
	}
}

/* Create the kprintf lock. Must be called before creating a second thread. */
void
kprintf_bootstrap(void)
{
	assert(kprintf_lock == NULL);

	kprintf_lock = lock_create("kprintf_lock");
	if (kprintf_lock == NULL) {
		panic("Could not create kprintf lock\n");
	}
}

/* Printf to the console. */
int
kprintf(const char *fmt, ...)
{
	int chars;
	va_list ap;

	if (kprintf_lock != NULL && !in_interrupt && curspl==0) {
		lock_acquire(kprintf_lock);
	}

	va_start(ap, fmt);
	chars = __vprintf(console_send, NULL, fmt, ap);
	va_end(ap);

	if (kprintf_lock != NULL && !in_interrupt && curspl==0) {
		lock_release(kprintf_lock);
	}

	return chars;
}

/*
 * panic() is for fatal errors. It prints the printf arguments it's
 * passed and then halts the system.
 */

void
panic(const char *fmt, ...)
{
	va_list ap;

	/*
	 * When we reach panic, the system is usually fairly screwed up.
	 * It's not entirely uncommon for anything else we try to do 
	 * here to trigger more panics.
	 *
	 * This variable makes sure that if we try to do something here,
	 * and it causes another panic, *that* panic doesn't try again;
	 * trying again almost inevitably causes infinite recursion.
	 *
	 * This is not excessively paranoid - these things DO happen!
	 */
	static volatile int evil;

	if (evil==0) {
		evil = 1;

		/*
		 * Not only do we not want to be interrupted while
		 * panicking, but we also want the console to be
		 * printing in polling mode so as not to do context
		 * switches. So turn interrupts off.
		 */
		splhigh();
	}

	if (evil==1) {
		evil = 2;

		thread_panic();
	}

	if (evil==2) {
		evil = 3;

		kprintf("panic: ");
		va_start(ap, fmt);
		__vprintf(console_send, NULL, fmt, ap);
		va_end(ap);
	}

	if (evil==3) {
		evil = 4;

		vfs_sync();
	}

	if (evil==4) {
		evil = 5;

		md_panic();
	}

	/*
	 * Last resort, just in case.
	 */

	for (;;);
}
