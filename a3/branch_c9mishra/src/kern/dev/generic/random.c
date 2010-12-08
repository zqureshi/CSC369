#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <lib.h>
#include <uio.h>
#include <vfs.h>
#include <generic/random.h>
#include "autoconf.h"

/*
 * Machine-independent generic randomness device.
 *
 * Remembers something that's a random source, and provides random()
 * and randmax() to the rest of the kernel.
 *
 * The kernel config mechanism can be used to explicitly choose which
 * of the available random sources to use, if more than one is
 * available.
 */

static struct random_softc *the_random = NULL;

/*
 * VFS device functions.
 * open: allow reading only.
 */
static
int
randopen(struct device *dev, int openflags)
{
	(void)dev;

	if (openflags != O_RDONLY) {
		return EIO;
	}

	return 0;
}

/*
 * VFS close function.
 */
static
int
randclose(struct device *dev)
{
	(void)dev;
	return 0;
}

/*
 * VFS I/O function. Hand off to implementation.
 */
static
int
randio(struct device *dev, struct uio *uio)
{
	struct random_softc *rs = dev->d_data;

	if (uio->uio_rw != UIO_READ) {
		return EIO;
	}

	return rs->rs_read(rs->rs_devdata, uio);
}

/*
 * VFS ioctl function.
 */
static
int
randioctl(struct device *dev, int op, userptr_t data)
{
	/*
	 * We don't support any ioctls.
	 */
	(void)dev;
	(void)op;
	(void)data;
	return EIOCTL;
}

/*
 * Config function.
 */
int
config_random(struct random_softc *rs, int unit)
{
	int result;

	/* We use only the first random device. */
	if (unit!=0) {
		return ENODEV;
	}

	assert(the_random==NULL);
	the_random = rs;

	rs->rs_dev.d_open = randopen;
	rs->rs_dev.d_close = randclose;
	rs->rs_dev.d_io = randio;
	rs->rs_dev.d_ioctl = randioctl;
	rs->rs_dev.d_blocks = 0;
	rs->rs_dev.d_blocksize = 1;
	rs->rs_dev.d_data = rs;

	/* Add the VFS device structure to the VFS device list. */
	result = vfs_adddev("random", &rs->rs_dev, 0);
	if (result) {
		return result;
	}

	return 0;
}


/*
 * Random number functions exported to the rest of the kernel.
 */

u_int32_t
random(void)
{
	if (the_random==NULL) {
		panic("No random device\n");
	}
	return the_random->rs_random(the_random->rs_devdata);
}

u_int32_t
randmax(void)
{
	if (the_random==NULL) {
		panic("No random device\n");
	}
	return the_random->rs_randmax(the_random->rs_devdata);
}
