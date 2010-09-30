#ifndef _FS_H_
#define _FS_H_

/*
 * Abstract filesystem. (Or device accessible as a file.)
 *
 * Operations:
 *
 *      fs_sync       - Flush all dirty buffers to disk.
 *      fs_getvolname - Return volume name of filesystem.
 *      fs_getroot    - Return root vnode of filesystem.
 *      fs_unmount    - Attempt unmount of filesystem.
 *
 * fs_getvolname may return NULL on filesystem types that don't
 * support the concept of a volume name. The string returned is
 * assumed to point into the filesystem's private storage and live
 * until unmount time.
 *
 * If the volume name changes on the fly, there is no way at present
 * to make sure such changes don't cause name conflicts. So it probably
 * should be considered fixed.
 *
 * fs_getroot should increment the refcount of the vnode returned.
 * It should not ever return NULL.
 *
 * If fs_unmount returns an error, the filesystem stays mounted, and
 * consequently the struct fs instance should remain valid. On success,
 * however, the filesystem object and all storage associated with the
 * filesystem should have been discarded/released.
 *
 * fs_data is a pointer to filesystem-specific data.
 */
struct fs {
	int           (*fs_sync)(struct fs *);
	const char   *(*fs_getvolname)(struct fs *);
	struct vnode *(*fs_getroot)(struct fs *);
	int           (*fs_unmount)(struct fs *);

	void *fs_data;
};

/*
 * Macros to shorten the calling sequences.
 */
#define FSOP_SYNC(fs)        ((fs)->fs_sync(fs))
#define FSOP_GETVOLNAME(fs)  ((fs)->fs_getvolname(fs))
#define FSOP_GETROOT(fs)     ((fs)->fs_getroot(fs))
#define FSOP_UNMOUNT(fs)     ((fs)->fs_unmount(fs))


#endif /* _FS_H_ */
