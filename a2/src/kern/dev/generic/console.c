/*
 * Machine (and hardware) independent console driver.
 *
 * We expose a simple interface to the rest of the kernel: "putch" to
 * print a character, "getch" to read one.
 *
 * As long as the device we're connected to does, we allow printing in
 * an interrupt handler or with interrupts off (by polling),
 * transparently to the caller. Note that getch by polling is not
 * supported, although such support could be added without undue
 * difficulty.
 *
 * Note that nothing happens until we have a device to write to. A
 * buffer of size DELAYBUFSIZE is used to hold output that is
 * generated before this point. This means that (1) using kprintf for
 * debugging problems that occur early in initialization is awkward,
 * and (2) if the system crashes before we find a console, no output
 * at all may appear.
 *
 * Note that we have no input buffering; characters typed too rapidly
 * will be lost.
 */

#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <machine/spl.h>
#include <synch.h>
#include <generic/console.h>
#include <dev.h>
#include <vfs.h>
#include <uio.h>
#include "autoconf.h"

/*
 * The console device.
 */
static struct con_softc *the_console = NULL;

/*
 * Lock so user I/Os are atomic.
 * We use two locks so readers waiting for input don't lock out writers.
 */
static struct lock *con_userlock_read = NULL;
static struct lock *con_userlock_write = NULL;

//////////////////////////////////////////////////

/*
 * This is for accumulating characters printed before the
 * console is set up. Upon console setup they are dumped 
 * to the actual console; thenceforth this space is unused.
 */
#define DELAYBUFSIZE  1024
static char delayed_outbuf[DELAYBUFSIZE];
static size_t delayed_outbuf_pos=0;

static
void
putch_delayed(int ch)
{
	/*
	 * No synchronization needed: called only during system startup
	 * by main thread.
	 */

	assert(delayed_outbuf_pos < sizeof(delayed_outbuf));
	delayed_outbuf[delayed_outbuf_pos++] = ch;
}

static
void
flush_delay_buf(void)
{
	size_t i;
	for (i=0; i<delayed_outbuf_pos; i++) {
		putch(delayed_outbuf[i]);
	}
	delayed_outbuf_pos = 0;
}

//////////////////////////////////////////////////

/*
 * Print a character, using polling instead of interrupts to wait for
 * I/O completion.
 */
static
void
putch_polled(struct con_softc *cs, int ch)
{
	cs->cs_sendpolled(cs->cs_devdata, ch);
}

//////////////////////////////////////////////////

/*
 * Print a character, using interrupts to wait for I/O completion.
 */

static
void
putch_intr(struct con_softc *cs, int ch)
{
	P(cs->cs_wsem);
	cs->cs_send(cs->cs_devdata, ch);
}

/*
 * Read a character, using interrupts to wait for I/O completion.
 */

static
int
getch_intr(struct con_softc *cs)
{
	P(cs->cs_rsem);
	return cs->cs_gotchar;
}

/*
 * Called from underlying device when a read-ready interrupt occurs.
 */
void
con_input(void *vcs, int ch)
{
	struct con_softc *cs = vcs;

	cs->cs_gotchar = ch;
	V(cs->cs_rsem);
}

/*
 * Called from underlying device when a write-done interrupt occurs.
 */
void
con_start(void *vcs)
{
	struct con_softc *cs = vcs;

	V(cs->cs_wsem);
}

//////////////////////////////////////////////////

/*
 * Exported interface.
 * 
 * Warning: putch must work even in an interrupt handler or with
 * interrupts disabled, and before the console is probed. getch need
 * not, and does not.
 */

void
putch(int ch)
{
	struct con_softc *cs = the_console;

	if (cs==NULL) {
		putch_delayed(ch);
	}
	else if (in_interrupt || curspl>0) {
		putch_polled(cs, ch);
	}
	else {
		putch_intr(cs, ch);
	}
}

int
getch(void)
{
	struct con_softc *cs = the_console;
	assert(cs!=NULL);
	assert(!in_interrupt && curspl==0);

	return getch_intr(cs);
}

////////////////////////////////////////////////////////////

/*
 * VFS interface functions
 */

static
int
con_open(struct device *dev, int openflags)
{
	(void)dev;
	(void)openflags;
	return 0;
}

static
int
con_close(struct device *dev)
{
	(void)dev;
	return 0;
}

static
int
con_io(struct device *dev, struct uio *uio)
{
	int result;
	char ch;
	struct lock *lk;

	(void)dev;  // unused

	if (uio->uio_rw==UIO_READ) {
		lk = con_userlock_read;
	}
	else {
		lk = con_userlock_write;
	}

	assert(lk != NULL);
	lock_acquire(lk);

	while (uio->uio_resid > 0) {
		if (uio->uio_rw==UIO_READ) {
			ch = getch();
			if (ch=='\r') {
				ch = '\n';
			}
			result = uiomove(&ch, 1, uio);
			if (result) {
				lock_release(lk);
				return result;
			}
			if (ch=='\n') {
				break;
			}
		}
		else {
			result = uiomove(&ch, 1, uio);
			if (result) {
				lock_release(lk);
				return result;
			}
			if (ch=='\n') {
				putch('\r');
			}
			putch(ch);
		}
	}
	lock_release(lk);
	return 0;
}

static
int
con_ioctl(struct device *dev, int op, userptr_t data)
{
	/* No ioctls. */
	(void)dev;
	(void)op;
	(void)data;
	return EINVAL;
}

static
int
attach_console_to_vfs(struct con_softc *cs)
{
	struct device *dev;
	int result;

	dev = kmalloc(sizeof(*dev));
	if (dev==NULL) {
		return ENOMEM;
	}

	dev->d_open = con_open;
	dev->d_close = con_close;
	dev->d_io = con_io;
	dev->d_ioctl = con_ioctl;
	dev->d_blocks = 0;
	dev->d_blocksize = 1;
	dev->d_data = cs;

	result = vfs_adddev("con", dev, 0);
	if (result) {
		kfree(dev);
		return result;
	}

	return 0;
}

////////////////////////////////////////////////////////////

/*
 * Config routine called by autoconf.c after we are attached to something.
 */

int
config_con(struct con_softc *cs, int unit)
{
	struct semaphore *rsem, *wsem;
	struct lock *rlk, *wlk;

	/*
	 * Only allow one system console.
	 * Further devices that could be the system console are ignored.
	 *
	 * Do not hardwire the console to be "con1" instead of "con0",
	 * or these asserts will go off.
	 */
	if (unit>0) {
		assert(the_console!=NULL);
		return ENODEV;
	}
	assert(the_console==NULL);

	rsem = sem_create("console read", 0);
	if (rsem == NULL) {
		return ENOMEM;
	}
	wsem = sem_create("console write", 1);
	if (wsem == NULL) {
		sem_destroy(rsem);
		return ENOMEM;
	}
	rlk = lock_create("console-lock-read");
	if (rlk == NULL) {
		sem_destroy(rsem);
		sem_destroy(wsem);
		return ENOMEM;
	}
	wlk = lock_create("console-lock-write");
	if (wlk == NULL) {
		lock_destroy(rlk);
		sem_destroy(rsem);
		sem_destroy(wsem);
		return ENOMEM;
	}

	cs->cs_rsem = rsem; 
	cs->cs_wsem = wsem; 
	cs->cs_gotchar = 0;

	the_console = cs;
	con_userlock_read = rlk;
	con_userlock_write = wlk;

	flush_delay_buf();

	return attach_console_to_vfs(cs);
}
