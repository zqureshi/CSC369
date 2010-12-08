#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <uio.h>
#include <sfs.h>
#include <dev.h>

////////////////////////////////////////////////////////////
//
// Basic block-level I/O routines
//
// Note: sfs_rblock is used to read the superblock
// early in mount, before sfs is fully (or even mostly)
// initialized, and so may not use anything from sfs
// except sfs_device.

int
sfs_rwblock(struct sfs_fs *sfs, struct uio *uio)
{
	int result;
	int tries=0;

	DEBUG(DB_SFS, "sfs: %s %u\n", 
	      uio->uio_rw == UIO_READ ? "read" : "write",
	      uio->uio_offset / SFS_BLOCKSIZE);

 retry:
	result = sfs->sfs_device->d_io(sfs->sfs_device, uio);
	if (result == EINVAL) {
		/*
		 * This means the sector we requested was out of range,
		 * or the seek address we gave wasn't sector-aligned,
		 * or a couple of other things that are our fault.
		 */
		panic("sfs: d_io returned EINVAL\n");
	}
	if (result == EIO) {
		if (tries == 0) {
			tries++;
			kprintf("sfs: block %u I/O error, retrying\n",
				uio->uio_offset / SFS_BLOCKSIZE);
			goto retry;
		}
		else if (tries < 10) {
			tries++;
			goto retry;
		}
		else {
			kprintf("sfs: block %u I/O error, giving up after "
				"%d retries\n",
				uio->uio_offset / SFS_BLOCKSIZE, tries);
		}
	}
	return result;
}

int
sfs_rblock(struct sfs_fs *sfs, void *data, u_int32_t block)
{
	struct uio ku;
	SFSUIO(&ku, data, block, UIO_READ);
	return sfs_rwblock(sfs, &ku);
}

int
sfs_wblock(struct sfs_fs *sfs, void *data, u_int32_t block)
{
	struct uio ku;
	SFSUIO(&ku, data, block, UIO_WRITE);
	return sfs_rwblock(sfs, &ku);
}
