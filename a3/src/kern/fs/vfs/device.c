/*
 * Vnode operations for VFS devices.
 *
 * These hand off to the functions in the VFS device structure (see dev.h)
 * but take care of a bunch of common tasks in a uniform fashion.
 */
#include <types.h>
#include <lib.h>
#include <synch.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/stat.h>
#include <vnode.h>
#include <uio.h>
#include <dev.h>

/*
 * Called for each open().
 *
 * We reject O_APPEND.
 */
static
int
dev_open(struct vnode *v, int flags)
{
	struct device *d = v->vn_data;

	if (flags & (O_CREAT | O_TRUNC | O_EXCL | O_APPEND)) {
		return EINVAL;
	}

	return d->d_open(d, flags);
}

/*
 * Called on the last close().
 * Just pass through.
 */
static
int
dev_close(struct vnode *v)
{
	struct device *d = v->vn_data;
	return d->d_close(d);
}

/*
 * Called when the vnode refcount reaches zero.
 * Do nothing; devices are permanent.
 */
static
int
dev_reclaim(struct vnode *v)
{
	(void)v;
	/* nothing - device continues to exist even when not in use */
	return 0;
}

/*
 * Called for read. Hand off to d_io.
 */
static
int
dev_read(struct vnode *v, struct uio *uio)
{
	struct device *d = v->vn_data;
	assert(uio->uio_rw == UIO_READ);
	return d->d_io(d, uio);
}

/*
 * Used for several functions with the same type signature that are
 * not meaningful on devices.
 */
static
int
null_io(struct vnode *v, struct uio *uio)
{
	(void)v;
	(void)uio;
	return EINVAL;
}

/*
 * Called for write. Hand off to d_io.
 */
static
int
dev_write(struct vnode *v, struct uio *uio)
{
	struct device *d = v->vn_data;
	assert(uio->uio_rw == UIO_WRITE);
	return d->d_io(d, uio);
}

/*
 * Called for ioctl(). Just pass through.
 */
static
int
dev_ioctl(struct vnode *v, int op, userptr_t data)
{
	struct device *d = v->vn_data;
	return d->d_ioctl(d, op, data);
}

/*
 * Called for stat().
 * Set the type and the size (block devices only).
 * The link count for a device is always 1.
 */
static
int
dev_stat(struct vnode *v, struct stat *statbuf)
{
	struct device *d = v->vn_data;
	int result;

	bzero(statbuf, sizeof(struct stat));

	result = VOP_GETTYPE(v, &statbuf->st_mode);
	if (result) {
		return result;
	}

	statbuf->st_nlink = 1;
	statbuf->st_blocks = d->d_blocks;

	if (d->d_blocks > 0) {
		statbuf->st_size = d->d_blocks * d->d_blocksize;
	}
	else {
		statbuf->st_size = 0;
	}

	return 0;
}

/*
 * Return the type. A device is a "block device" if it has a known
 * length. A device that generates data in a stream is a "character
 * device".
 */
static
int
dev_gettype(struct vnode *v, u_int32_t *ret)
{
	struct device *d = v->vn_data;
	if (d->d_blocks > 0) {
		*ret = S_IFBLK;
	}
	else {
		*ret = S_IFCHR;
	}
	return 0;
}

/*
 * Attempt a seek.
 * For block devices, require block alignment.
 * For character devices, prohibit seeking entirely.
 */
static
int
dev_tryseek(struct vnode *v, off_t pos)
{
	struct device *d = v->vn_data;
	if (d->d_blocks > 0) {
		if ((pos % d->d_blocksize)!=0) {
			/* not block-aligned */
			return EINVAL;
		}
		if (pos < 0) {
			/* 
			 * Nonsensical.
			 * (note: off_t must be signed for SEEK_CUR or
			 * SEEK_END seeks to work, so this case must
			 * be checked.)
			 */
			return EINVAL;
		}
		if (pos / d->d_blocksize >= d->d_blocks) {
			/* off the end */
			return EINVAL;
		}
	}
	else {
		return ESPIPE;
	}
	return 0;
}

/*
 * For fsync() - meaningless, do nothing.
 */
static
int
null_fsync(struct vnode *v)
{
	(void)v;
	return 0;
}

/*
 * For mmap. If you want this to do anything, you have to write it
 * yourself. Some devices may not make sense to map. Others do.
 */
static
int
dev_mmap(struct vnode *v  /* add stuff as needed */)
{
	(void)v;
	return EUNIMP;
}

/*
 * For ftruncate(). 
 */
static
int
dev_truncate(struct vnode *v, off_t len)
{
	struct device *d = v->vn_data;

	/*
	 * Allow truncating to the object's own size, if it has one.
	 */
	if (d->d_blocks > 0 && (off_t)(d->d_blocks*d->d_blocksize) == len) {
		return 0;
	}

	return EINVAL;
}

/*
 * For namefile (which implements "pwd")
 *
 * This should never be reached, as it's not possible to chdir to a
 * device vnode.
 */
static
int
dev_namefile(struct vnode *v, struct uio *uio)
{
	/*
	 * The name of a device is always just "device:". The VFS
	 * layer puts in the device name for us, so we don't need to
	 * do anything further.
	 */

	(void)v;
	(void)uio;

	return 0;
}

/*
 * Operations that are completely meaningless on devices.
 */

static
int
null_creat(struct vnode *v, const char *name, int excl, struct vnode **result)
{
	(void)v;
	(void)name;
	(void)excl;
	(void)result;
	return ENOTDIR;
}

static
int
null_symlink(struct vnode *v, const char *contents, const char *name)
{
	(void)v;
	(void)contents;
	(void)name;
	return ENOTDIR;
}

static
int
null_nameop(struct vnode *v, const char *name)
{
	(void)v;
	(void)name;
	return ENOTDIR;
}

static
int
null_link(struct vnode *v, const char *name, struct vnode *file)
{
	(void)v;
	(void)name;
	(void)file;
	return ENOTDIR;
}

static
int
null_rename(struct vnode *v, const char *n1, struct vnode *v2, const char *n2)
{
	(void)v;
	(void)n1;
	(void)v2;
	(void)n2;
	return ENOTDIR;
}


/*
 * Name lookup.
 *
 * One interesting feature of device:name pathname syntax is that you can
 * implement pathnames on arbitrary devices. For instance, if you had a
 * serial port that actually let you control the RS232 settings (unlike
 * the LAMEbus serial port), you might arrange things so that you could
 * open it with pathnames like "ser:9600/n/8/1" in order to select the
 * operating mode.
 *
 * However, we have no support for this in the base system.
 */
static
int
dev_lookup(struct vnode *dir, 
	   char *pathname, struct vnode **result)
{
	/*
	 * If the path was "device:", we get "". For that, return self.
	 * Anything else is an error.
	 * Increment the ref count of the vnode before returning it.
	 */
	if (strlen(pathname)>0) {
		return ENOENT;
	}
	VOP_INCREF(dir);
	*result = dir;
	return 0;
}

static
int
dev_lookparent(struct vnode *dir, 
	       char *pathname, struct vnode **result,
	       char *namebuf, size_t buflen)
{
	/*
	 * This is always an error.
	 */
	(void)dir;
	(void)pathname;
	(void)result;
	(void)namebuf;
	(void)buflen;

	return ENOTDIR;
}

/*
 * Function table for device vnodes.
 */
static const struct vnode_ops dev_vnode_ops = {
	VOP_MAGIC,

	dev_open,
	dev_close,
	dev_reclaim,
	dev_read,
	null_io,      /* readlink */
	null_io,      /* getdirentry */
	dev_write,
	dev_ioctl,
	dev_stat,
	dev_gettype,
	dev_tryseek,
	null_fsync,
	dev_mmap,
	dev_truncate,
	dev_namefile,
	null_creat,
	null_symlink,
	null_nameop,  /* mkdir */
	null_link,
	null_nameop,  /* remove */
	null_nameop,  /* rmdir */
	null_rename,
	dev_lookup,
	dev_lookparent,
};

/*
 * Function to create a vnode for a VFS device.
 */
struct vnode *
dev_create_vnode(struct device *dev)
{
	int result;
	struct vnode *v;

	v = kmalloc(sizeof(struct vnode));
	if (v==NULL) {
		return NULL;
	}

	result = VOP_INIT(v, &dev_vnode_ops, NULL, dev);
	if (result != 0) {
		panic("While creating vnode for device: VOP_INIT: %s\n",
		      strerror(result));
	}

	return v;
}
