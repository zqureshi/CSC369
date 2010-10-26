/*
 * High-level VFS operations on pathnames.
 */

#include <types.h>
#include <kern/errno.h>
#include <kern/limits.h>
#include <kern/unistd.h>
#include <vfs.h>
#include <vnode.h>
#include <lib.h>


/* Does most of the work for open(). */
int
vfs_open(char *path, int openflags, struct vnode **ret)
{
	int how;
	int result;
	int canwrite;
	struct vnode *vn = NULL;

	how = openflags & O_ACCMODE;

	switch (how) {
	    case O_RDONLY:
		canwrite=0;
		break;
	    case O_WRONLY:
	    case O_RDWR:
		canwrite=1;
		break;
	    default:
		return EINVAL;
	}

	if (openflags & O_CREAT) {
		char name[NAME_MAX+1];
		struct vnode *dir;
		int excl = (openflags & O_EXCL)!=0;
		
		result = vfs_lookparent(path, &dir, name, sizeof(name));
		if (result) {
			return result;
		}

		result = VOP_CREAT(dir, name, excl, &vn);

		VOP_DECREF(dir);
	}
	else {
		result = vfs_lookup(path, &vn);
	}

	if (result) {
		return result;
	}

	assert(vn != NULL);

	result = VOP_OPEN(vn, openflags);
	if (result) {
		VOP_DECREF(vn);
		return result;
	}

	VOP_INCOPEN(vn);
	
	if (openflags & O_TRUNC) {
		if (canwrite==0) {
			result = EINVAL;
		}
		else {
			result = VOP_TRUNCATE(vn, 0);
		}
		if (result) {
			VOP_DECOPEN(vn);
			VOP_DECREF(vn);
			return result;
		}
	}

	*ret = vn;

	return 0;
}

/* Does most of the work for close(). */
void
vfs_close(struct vnode *vn)
{
	/*
	 * VOP_DECOPEN and VOP_DECREF don't return errors.
	 *
	 * We assume that the file system makes every reasonable
	 * effort to not fail. If it does fail - such as on a hard I/O
	 * error or something - vnode.c prints a warning. The reason
	 * we don't report errors up to or above this level is that
	 *    (1) most application software does not check for close
	 *        failing, and more importantly
	 *    (2) we're often called from places like process exit
	 *        where reporting the error is impossible and
	 *        meaningful recovery is entirely impractical.
	 */

	VOP_DECOPEN(vn);
	VOP_DECREF(vn);
}

/* Does most of the work for remove(). */
int
vfs_remove(char *path)
{
	struct vnode *dir;
	char name[NAME_MAX+1];
	int result;
	
	result = vfs_lookparent(path, &dir, name, sizeof(name));
	if (result) {
		return result;
	}

	result = VOP_REMOVE(dir, name);
	VOP_DECREF(dir);

	return result;
}

/* Does most of the work for rename(). */
int
vfs_rename(char *oldpath, char *newpath)
{
	struct vnode *olddir;
	char oldname[NAME_MAX+1];
	struct vnode *newdir;
	char newname[NAME_MAX+1];
	int result;
	
	result = vfs_lookparent(oldpath, &olddir, oldname, sizeof(oldname));
	if (result) {
		return result;
	}
	result = vfs_lookparent(newpath, &newdir, newname, sizeof(newname));
	if (result) {
		VOP_DECREF(olddir);
		return result;
	}

	if (olddir->vn_fs==NULL || newdir->vn_fs==NULL || 
	    olddir->vn_fs != newdir->vn_fs) {
		VOP_DECREF(newdir);
		VOP_DECREF(olddir);
		return EXDEV;
	}

	result = VOP_RENAME(olddir, oldname, newdir, newname);

	VOP_DECREF(newdir);
	VOP_DECREF(olddir);

	return result;
}

/* Does most of the work for link(). */
int
vfs_link(char *oldpath, char *newpath)
{
	struct vnode *oldfile;
	struct vnode *newdir;
	char newname[NAME_MAX+1];
	int result;

	result = vfs_lookup(oldpath, &oldfile);
	if (result) {
		return result;
	}
	result = vfs_lookparent(newpath, &newdir, newname, sizeof(newname));
	if (result) {
		VOP_DECREF(oldfile);
		return result;
	}

	if (oldfile->vn_fs==NULL || newdir->vn_fs==NULL ||
	    oldfile->vn_fs != newdir->vn_fs) {
		VOP_DECREF(newdir);
		VOP_DECREF(oldfile);
		return EXDEV;
	}

	result = VOP_LINK(newdir, newname, oldfile);

	VOP_DECREF(newdir);
	VOP_DECREF(oldfile);

	return result;
}

/*
 * Does most of the work for symlink().
 *
 * Note, however, if you're implementing symlinks, that various
 * other parts of the VFS layer are missing crucial elements of
 * support for symlinks.
 */
int
vfs_symlink(const char *contents, char *path)
{
	struct vnode *newdir;
	char newname[NAME_MAX+1];
	int result;

	result = vfs_lookparent(path, &newdir, newname, sizeof(newname));
	if (result) {
		return result;
	}

	result = VOP_SYMLINK(newdir, newname, contents);
	VOP_DECREF(newdir);

	return result;
}

/*
 * Does most of the work for readlink().
 *
 * Note, however, if you're implementing symlinks, that various
 * other parts of the VFS layer are missing crucial elements of
 * support for symlinks.
 */
int
vfs_readlink(char *path, struct uio *uio)
{
	struct vnode *vn;
	int result;

	result = vfs_lookup(path, &vn);
	if (result) {
		return result;
	}

	result = VOP_READLINK(vn, uio);

	VOP_DECREF(vn);

	return result;
}

/*
 * Does most of the work for mkdir.
 */
int
vfs_mkdir(char *path)
{
	struct vnode *parent;
	char name[NAME_MAX+1];
	int result;

	result = vfs_lookparent(path, &parent, name, sizeof(name));
	if (result) {
		return result;
	}

	result = VOP_MKDIR(parent, name);

	VOP_DECREF(parent);

	return result;
}

/*
 * Does most of the work for rmdir.
 */
int
vfs_rmdir(char *path)
{
	struct vnode *parent;
	char name[NAME_MAX+1];
	int result;

	result = vfs_lookparent(path, &parent, name, sizeof(name));
	if (result) {
		return result;
	}

	result = VOP_RMDIR(parent, name);

	VOP_DECREF(parent);

	return result;
}

