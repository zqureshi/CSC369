/*
 * Implementation of the null device, "null:", which generates an
 * immediate EOF on read and throws away anything written to it.
 */
#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <vfs.h>
#include <dev.h>
#include <uio.h>

/* For open() */
static
int
nullopen(struct device *dev, int openflags)
{
	(void)dev;
	(void)openflags;

	return 0;
}

/* For close() */
static
int
nullclose(struct device *dev)
{
	(void)dev;
	return 0;
}

/* For d_io() */
static
int
nullio(struct device *dev, struct uio *uio)
{
	/*
	 * On write, discard everything without looking at it.
	 * On read, do nothing, generating an immediate EOF.
	 */

	(void)dev; // unused

	if (uio->uio_rw == UIO_WRITE) {
		uio->uio_resid = 0;
	}

	return 0;
}

/* For ioctl() */
static
int
nullioctl(struct device *dev, int op, userptr_t data)
{
	/*
	 * No ioctls.
	 */

	(void)dev;
	(void)op;
	(void)data;

	return EINVAL;
}

/*
 * Function to create and attach null:
 */
void
devnull_create(void)
{
	int result;
	struct device *dev;

	dev = kmalloc(sizeof(*dev));
	if (dev==NULL) {
		panic("Could not add null device: out of memory\n");
	}

	
	dev->d_open = nullopen;
	dev->d_close = nullclose;
	dev->d_io = nullio;
	dev->d_ioctl = nullioctl;

	dev->d_blocks = 0;
	dev->d_blocksize = 1;

	dev->d_data = NULL;

	result = vfs_adddev("null", dev, 0);
	if (result) {
		panic("Could not add null device: %s\n", strerror(result));
	}
}
