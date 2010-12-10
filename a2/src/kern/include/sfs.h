#ifndef _SFS_H_
#define _SFS_H_

/*
 * Get abstract structure definitions
 */
#include <vnode.h>
#include <fs.h>

/*
 * Get on-disk structures and constants that are made available to 
 * userland for the benefit of mksfs, dumpsfs, etc.
 */
#include <kern/sfs.h>

struct sfs_vnode {
	struct vnode sv_v;              /* abstract vnode structure */
	struct sfs_inode sv_i;		/* on-disk inode */
	u_int32_t sv_ino;               /* inode number */
	int sv_dirty;                   /* true if sv_i modified */
};

struct sfs_fs {
	struct fs sfs_absfs;            /* abstract filesystem structure */
	struct sfs_super sfs_super;	/* on-disk superblock */
	int sfs_superdirty;             /* true if superblock modified */
	struct device *sfs_device;      /* device mounted on */
	struct array *sfs_vnodes;       /* vnodes loaded into memory */
	struct bitmap *sfs_freemap;     /* blocks in use are marked 1 */
	int sfs_freemapdirty;           /* true if freemap modified */
};

/*
 * Function for mounting a sfs (calls vfs_mount)
 */
int sfs_mount(const char *device);


/*
 * Internal functions
 */

/* Initialize uio structure */
#define SFSUIO(uio, ptr, block, rw) \
    mk_kuio(uio, ptr, SFS_BLOCKSIZE, ((off_t)(block))*SFS_BLOCKSIZE, rw)

/* Convenience functions for block I/O */
int sfs_rwblock(struct sfs_fs *sfs, struct uio *uio);
int sfs_rblock(struct sfs_fs *sfs, void *data, u_int32_t block);
int sfs_wblock(struct sfs_fs *sfs, void *data, u_int32_t block);

/* Get root vnode */
struct vnode *sfs_getroot(struct fs *fs);

#endif /* _SFS_H_ */
