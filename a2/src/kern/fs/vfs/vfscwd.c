/*
 * VFS operations involving the current directory.
 */

#include <types.h>
#include <kern/errno.h>
#include <kern/stat.h>
#include <lib.h>
#include <vfs.h>
#include <fs.h>
#include <vnode.h>
#include <uio.h>
#include <thread.h>
#include <curthread.h>

/*
 * Get current directory as a vnode.
 * 
 * We do not synchronize curthread->t_cwd, because it belongs exclusively
 * to its own thread; no other threads should access it.
 */
int
vfs_getcurdir(struct vnode **ret)
{
	int rv = 0;

	if (curthread->t_cwd!=NULL) {
		VOP_INCREF(curthread->t_cwd);
		*ret = curthread->t_cwd;
	}
	else {
		rv = ENOENT;
	}

	return rv;
}

/*
 * Set current directory as a vnode.
 * The passed vnode must in fact be a directory.
 */
int
vfs_setcurdir(struct vnode *dir)
{
	struct vnode *old;
	u_int32_t vtype;
	int result;

	result = VOP_GETTYPE(dir, &vtype);
	if (result) {
		return result;
	}
	if (!S_ISDIR(vtype)) {
		return ENOTDIR;
	}

	VOP_INCREF(dir);

	old = curthread->t_cwd;
	curthread->t_cwd = dir;

	if (old!=NULL) { 
		VOP_DECREF(old);
	}

	return 0;
}

/*
 * Set current directory to "none".
 */
int
vfs_clearcurdir(void)
{
	struct vnode *old;

	old = curthread->t_cwd;
	curthread->t_cwd = NULL;

	if (old!=NULL) { 
		VOP_DECREF(old);
	}

	return 0;
}

/*
 * Set current directory, as a pathname. Use vfs_lookup to translate
 * it to a vnode.
 */
int
vfs_chdir(char *path)
{
	struct vnode *vn;
	int result;

	result = vfs_lookup(path, &vn);
	if (result) {
		return result;
	}
	result = vfs_setcurdir(vn);
	VOP_DECREF(vn);
	return result;
}

/*
 * Get current directory, as a pathname.
 * Use VOP_NAMEFILE to get the pathname and FSOP_GETVOLNAME to get the
 * volume name.
 */
int
vfs_getcwd(struct uio *uio)
{
	struct vnode *cwd;
	int result;
	const char *name;
	char colon=':';

	assert(uio->uio_rw==UIO_READ);

	result = vfs_getcurdir(&cwd);
	if (result) {
		return result;
	}

	/* The current dir must be a directory, and thus it is not a device. */
	assert(cwd->vn_fs != NULL);

	name = FSOP_GETVOLNAME(cwd->vn_fs);
	if (name==NULL) {
		name = vfs_getdevname(cwd->vn_fs);
	}
	assert(name != NULL);

	result = uiomove((char *)name, strlen(name), uio);
	if (result) {
		goto out;
	}
	result = uiomove(&colon, 1, uio);
	if (result) {
		goto out;
	}

	result = VOP_NAMEFILE(cwd, uio);

 out:

	VOP_DECREF(cwd);
	return result;
}
