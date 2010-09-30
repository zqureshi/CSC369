#ifndef _EMUFS_H_
#define _EMUFS_H_

/*
 * Get abstract structure definitions
 */
#include <vnode.h>
#include <fs.h>

/*
 * Our structures
 */

struct emufs_vnode {
	struct vnode ev_v;		/* abstract vnode structure */
	struct emu_softc *ev_emu;	/* device */
	u_int32_t ev_handle;		/* file handle */
};

struct emufs_fs {
	struct fs ef_fs;		/* abstract filesystem structure */
	struct emu_softc *ef_emu;	/* device */
	struct emufs_vnode *ef_root;	/* root vnode */
	struct array *ef_vnodes;	/* table of loaded vnodes */
};

#endif /* _EMUFS_H_ */
