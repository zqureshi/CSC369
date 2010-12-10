/*
 * SFS filesystem
 *
 * Filesystem-level interface routines.
 */

#include <types.h>
#include <lib.h>
#include <synch.h>
#include <kern/errno.h>
#include <array.h>
#include <bitmap.h>
#include <uio.h>
#include <dev.h>
#include <sfs.h>
#include <vfs.h>

/* Shortcuts for the size macros in kern/sfs.h */
#define SFS_FS_BITMAPSIZE(sfs)  SFS_BITMAPSIZE((sfs)->sfs_super.sp_nblocks)
#define SFS_FS_BITBLOCKS(sfs)   SFS_BITBLOCKS((sfs)->sfs_super.sp_nblocks)

/*
 * Routine for doing I/O (reads or writes) on the free block bitmap.
 * We always do the whole bitmap at once; writing individual sectors
 * might or might not be a worthwhile optimization.
 *
 * The free block bitmap consists of SFS_BITBLOCKS 512-byte sectors of
 * bits, one bit for each sector on the filesystem. The number of
 * blocks in the bitmap is thus rounded up to the nearest multiple of
 * 512*8 = 4096. (This rounded number is SFS_BITMAPSIZE.) This means
 * that the bitmap will (in general) contain space for some number of
 * invalid sectors that are actually beyond the end of the disk
 * device. This is ok. These sectors are supposed to be marked "in
 * use" by mksfs and never get marked "free".
 *
 * The sectors used by the superblock and the bitmap itself are
 * likewise marked in use by mksfs.
 *
 * Locking: Must hold bitlock. Nothing acquired or released.
 */

static
int
sfs_mapio(struct sfs_fs *sfs, enum uio_rw rw)
{
	u_int32_t j, mapsize;
	char *bitdata;
	int result;

	assert(lock_do_i_hold(sfs->sfs_bitlock));

	/* Number of blocks in the bitmap. */
	mapsize = SFS_FS_BITBLOCKS(sfs);

	/* Pointer to our bitmap data in memory. */
	bitdata = bitmap_getdata(sfs->sfs_freemap);
	
	/* For each sector in the bitmap... */
	for (j=0; j<mapsize; j++) {

		/* Get a pointer to its data */
		void *ptr = bitdata + j*SFS_BLOCKSIZE;

		/* and read or write it. The bitmap starts at sector 2. */ 
		if (rw == UIO_READ) {
			result = sfs_rblock(sfs, ptr, SFS_MAP_LOCATION+j);
		}
		else {
			result = sfs_wblock(sfs, ptr, SFS_MAP_LOCATION+j);
		}

		/* If we failed, stop. */
		if (result) {
			return result;
		}
	}
	return 0;
}

/*
 * Sync routine. This is what gets invoked if you do FS_SYNC on the
 * sfs filesystem structure.
 *
 * Locking: gets sfs_vnlock, sfs_bitlock, and (via VOP_FSYNC), vnode locks,
 * but none at the same time.
 */

static
int
sfs_sync(struct fs *fs)
{
	struct sfs_fs *sfs; 
	int i, num, result;
	struct array *tmp;

	/*
	 * Get the sfs_fs from the generic abstract fs.
	 *
	 * Note that the abstract struct fs, which is all the VFS
	 * layer knows about, is actually a member of struct sfs_fs.
	 * The pointer in the struct fs points back to the top of the
	 * struct sfs_fs - essentially the same object. This can be a
	 * little confusing at first.
	 *
	 * The following diagram may help:
	 *
	 *     struct sfs_fs        <-------------\
         *           :                            |
         *           :   sfs_absfs (struct fs)    |   <------\
         *           :      :                     |          |
         *           :      :  various members    |          |
         *           :      :                     |          |
         *           :      :  fs_data  ----------/          |
         *           :      :                             ...|...
         *           :                                   .  VFS  .
         *           :                                   . layer . 
         *           :   other members                    .......
         *           :                                    
         *           :
	 *
	 * This construct is repeated with vnodes and devices and other
	 * similar things all over the place in OS/161, so taking the
	 * time to straighten it out in your mind is worthwhile.
	 */

	sfs = fs->fs_data;


	/*
	 * This is kind of a hack. We can't acquire vnode locks while
	 * holding sfs_vnlock, because that violates the ordering
	 * constraints (see sfs_vnode.c) - so we *copy* the array of
	 * loaded vnodes into a temporary array and sync those.
	 */

	tmp = array_create();
	if (tmp == NULL) {
		return ENOMEM;
	}
	lock_acquire(sfs->sfs_vnlock);

	/* Go over the array of loaded vnodes. */
	num = array_getnum(sfs->sfs_vnodes);
	for (i=0; i<num; i++) {
		struct sfs_vnode *sv = array_getguy(sfs->sfs_vnodes, i);
		VOP_INCREF(&sv->sv_v);
		if (array_add(tmp, sv) != 0) {
			// XXX
			panic("sfs_sync: array_add failed\n");
		}
	}

	lock_release(sfs->sfs_vnlock);

	/* Now sync. */
	num = array_getnum(tmp);
	for (i=0; i<num; i++) {
		struct sfs_vnode *sv = array_getguy(tmp, i);
		result = VOP_FSYNC(&sv->sv_v);
		if (result) {
			kprintf("SFS: Warning: syncing inode %d: %s\n",
				sv->sv_ino, strerror(result));
		}
		VOP_DECREF(&sv->sv_v);
	}
	array_destroy(tmp);

	lock_acquire(sfs->sfs_bitlock);

	/* If the free block map needs to be written, write it. */
	if (sfs->sfs_freemapdirty) {
		result = sfs_mapio(sfs, UIO_WRITE);
		if (result) {
			kprintf("SFS: Warning: syncing bitmap: %s\n",
				strerror(result));
		}
		else {
			/* Only clear the dirty bit if we succeeded */
			sfs->sfs_freemapdirty = 0;
		}
	}

	/* If the superblock needs to be written, write it. */
	if (sfs->sfs_superdirty) {
		result = sfs_wblock(sfs, &sfs->sfs_super, SFS_SB_LOCATION);
		if (result) {
			kprintf("SFS: Warning: syncing superblock: %s\n",
				strerror(result));
		}
		else {
			/* Only clear the dirty bit if we succeeded */
			sfs->sfs_superdirty = 0;
		}
	}

	lock_release(sfs->sfs_bitlock);

	return 0;
}

/*
 * Routine to retrieve the volume name. Filesystems can be referred
 * to by their volume name followed by a colon as well as the name
 * of the device they're mounted on.
 */
static
const char *
sfs_getvolname(struct fs *fs)
{
	struct sfs_fs *sfs = fs->fs_data;

	/*
	 * VFS only uses the volume name transiently, and its
	 * synchronization guarantees that we will not disappear while
	 * it's using the name. Furthermore, we don't permit the volume
	 * name to change on the fly (this is also a restriction in VFS)
	 * so there's no need to synchronize.
	 */

	return sfs->sfs_super.sp_volname;
}

/*
 * Unmount code.
 *
 * VFS calls FS_SYNC on the filesystem prior to unmounting it.
 *
 * Locking: gets sfs_vnlock, then sfs_bitlock.
 */
static
int
sfs_unmount(struct fs *fs)
{
	struct sfs_fs *sfs = fs->fs_data;


	lock_acquire(sfs->sfs_vnlock);
	lock_acquire(sfs->sfs_bitlock);
	
	/* Do we have any files open? If so, can't unmount. */
	if (array_getnum(sfs->sfs_vnodes)>0) {
		lock_release(sfs->sfs_vnlock);
		lock_release(sfs->sfs_bitlock);
		return EBUSY;
	}

	/*
	 * We should have just had sfs_sync called.
	 * The VFS locking prevents anyone from opening any files on the
	 * fs before we get here - in order to open any files, one would
	 * have to go through the volume/device name stuff in vfslist.c,
	 * and it's locked during the sync/unmount.
	 */
	assert(sfs->sfs_superdirty==0);
	assert(sfs->sfs_freemapdirty==0);

	/* Once we start nuking stuff we can't fail. */
	array_destroy(sfs->sfs_vnodes);
	bitmap_destroy(sfs->sfs_freemap);
	
	/* The vfs layer takes care of the device for us */
	(void)sfs->sfs_device;

	/* Free the lock. VFS guarantees we can do this safely */
	lock_release(sfs->sfs_vnlock);
	lock_release(sfs->sfs_bitlock);
	lock_destroy(sfs->sfs_vnlock);
	lock_destroy(sfs->sfs_bitlock);

	/* Destroy the fs object */
	kfree(sfs);

	/* nothing else to do */
	return 0;
}

/*
 * Mount routine.
 *
 * The way mount works is that you call vfs_mount and pass it a
 * filesystem-specific mount routine. This routine takes a device and
 * hands back a pointer to an abstract filesystem.  You can also pass
 * a void * through to the mount routine.
 *
 * This structure is necessitated by the fact that the VFS layer needs
 * to update its internal device tables atomically to avoid getting
 * two filesystems with the same name mounted at once, or two
 * filesystems mounted on the same device at once.
 *
 * Locking: creates and acquires the locks, then releases them when done.
 *   This is more for the benefit of assertions elsewhere than anything 
 *   else.
 */

static
int
sfs_domount(void *options, struct device *dev, struct fs **ret)
{
	int result;
	struct sfs_fs *sfs;

	/* We don't pass any options through mount */
	(void)options;

	/*
	 * Make sure our on-disk structures aren't messed up
	 */
	assert(sizeof(struct sfs_super)==SFS_BLOCKSIZE);
	assert(sizeof(struct sfs_inode)==SFS_BLOCKSIZE);
	assert(SFS_BLOCKSIZE % sizeof(struct sfs_dir) == 0);

	/*
	 * We can't mount on devices with the wrong sector size.
	 *
	 * (Note: for all intents and purposes here, "sector" and
	 * "block" are interchangeable terms. Technically a filesystem
	 * block may be composed of several hardware sectors, but we
	 * don't do that in sfs.)
	 */
	if (dev->d_blocksize != SFS_BLOCKSIZE) {
		return ENXIO;
	}

	/* Allocate object */
	sfs = kmalloc(sizeof(struct sfs_fs));
	if (sfs==NULL) {
		return ENOMEM;
	}

	/* Allocate array */
	sfs->sfs_vnodes = array_create();
	if (sfs->sfs_vnodes == NULL) {
		kfree(sfs);
		return ENOMEM;
	}

	/* Set the device so we can use sfs_rblock() */
	sfs->sfs_device = dev;

	/* Create and acquire the locks so various stuff works right */
	sfs->sfs_vnlock = lock_create("sfs_vnlock");
	if (sfs->sfs_vnlock == NULL) {
		array_destroy(sfs->sfs_vnodes);
		kfree(sfs);
		return ENOMEM;
	}

	sfs->sfs_bitlock = lock_create("sfs_bitlock");
	if (sfs->sfs_bitlock == NULL) {
		lock_destroy(sfs->sfs_vnlock);
		array_destroy(sfs->sfs_vnodes);
		kfree(sfs);
		return ENOMEM;
	}

	lock_acquire(sfs->sfs_vnlock);
	lock_acquire(sfs->sfs_bitlock);

	/* Load superblock */
	result = sfs_rblock(sfs, &sfs->sfs_super, SFS_SB_LOCATION);
	if (result) {
		lock_release(sfs->sfs_vnlock);
		lock_release(sfs->sfs_bitlock);
		lock_destroy(sfs->sfs_vnlock);
		lock_destroy(sfs->sfs_bitlock);
		array_destroy(sfs->sfs_vnodes);
		kfree(sfs);
		return result;
	}

	/* Make some simple sanity checks */

	if (sfs->sfs_super.sp_magic != SFS_MAGIC) {
		kprintf("sfs: Wrong magic number in superblock "
			"(0x%x, should be 0x%x)\n", 
			sfs->sfs_super.sp_magic,
			SFS_MAGIC);
		lock_release(sfs->sfs_vnlock);
		lock_release(sfs->sfs_bitlock);
		lock_destroy(sfs->sfs_vnlock);
		lock_destroy(sfs->sfs_bitlock);
		array_destroy(sfs->sfs_vnodes);
		kfree(sfs);
		return EINVAL;
	}
	
	if (sfs->sfs_super.sp_nblocks > dev->d_blocks) {
		kprintf("sfs: warning - fs has %u blocks, device has %u\n",
			sfs->sfs_super.sp_nblocks, dev->d_blocks);
	}

	/* Ensure null termination of the volume name */
	sfs->sfs_super.sp_volname[sizeof(sfs->sfs_super.sp_volname)-1] = 0;

	/* Load free space bitmap */
	sfs->sfs_freemap = bitmap_create(SFS_FS_BITMAPSIZE(sfs));
	if (sfs->sfs_freemap == NULL) {
		array_destroy(sfs->sfs_vnodes);
		kfree(sfs);
		return ENOMEM;
	}
	result = sfs_mapio(sfs, UIO_READ);
	if (result) {
		lock_release(sfs->sfs_vnlock);
		lock_release(sfs->sfs_bitlock);
		lock_destroy(sfs->sfs_vnlock);
		lock_destroy(sfs->sfs_bitlock);
		bitmap_destroy(sfs->sfs_freemap);
		array_destroy(sfs->sfs_vnodes);
		kfree(sfs);
		return result;
	}

	/* Set up abstract fs calls */
	sfs->sfs_absfs.fs_sync = sfs_sync;
	sfs->sfs_absfs.fs_getvolname = sfs_getvolname;
	sfs->sfs_absfs.fs_getroot = sfs_getroot;
	sfs->sfs_absfs.fs_unmount = sfs_unmount;

	sfs->sfs_absfs.fs_data = sfs;

	/* the other fields */
	sfs->sfs_superdirty = 0;
	sfs->sfs_freemapdirty = 0;

	/* Hand back the abstract fs */
	*ret = &sfs->sfs_absfs;

	lock_release(sfs->sfs_vnlock);
	lock_release(sfs->sfs_bitlock);

	return 0;
}

/*
 * Actual function called from high-level code to mount an sfs.
 */

int
sfs_mount(const char *device)
{
	return vfs_mount(device, NULL, sfs_domount);
}
