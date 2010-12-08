/*
 * Emulator passthrough filesystem.
 *
 * The idea is that this appears as a filesystem in the VFS layer, and
 * passes VFS operations through a somewhat complicated "hardware"
 * interface to some simulated "hardware" in System/161 that accesses
 * the filesystem System/161 is running in.
 *
 * This makes it unnecessary to copy the system files to the simulated
 * disk, although we recommend doing so and trying running without this
 * device as part of testing your filesystem.
 */

#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/stat.h>
#include <lib.h>
#include <synch.h>
#include <array.h>
#include <uio.h>
#include <vfs.h>
#include <emufs.h>
#include <lamebus/emu.h>
#include <machine/bus.h>
#include "autoconf.h"

/* Register offsets */
#define REG_HANDLE    0
#define REG_OFFSET    4
#define REG_IOLEN     8
#define REG_OPER      12
#define REG_RESULT    16

/* I/O buffer offset */
#define EMU_BUFFER    32768

/* Operation codes for REG_OPER */
#define EMU_OP_OPEN          1
#define EMU_OP_CREATE        2
#define EMU_OP_EXCLCREATE    3
#define EMU_OP_CLOSE         4
#define EMU_OP_READ          5
#define EMU_OP_READDIR       6
#define EMU_OP_WRITE         7
#define EMU_OP_GETSIZE       8
#define EMU_OP_TRUNC         9

/* Result codes for REG_RESULT */
#define EMU_RES_SUCCESS      1
#define EMU_RES_BADHANDLE    2
#define EMU_RES_BADOP        3
#define EMU_RES_BADPATH      4
#define EMU_RES_BADSIZE      5
#define EMU_RES_EXISTS       6
#define EMU_RES_ISDIR        7
#define EMU_RES_MEDIA        8
#define EMU_RES_NOHANDLES    9
#define EMU_RES_NOSPACE      10
#define EMU_RES_NOTDIR       11
#define EMU_RES_UNKNOWN      12
#define EMU_RES_UNSUPP       13

////////////////////////////////////////////////////////////
//
// Hardware ops
//

/*
 * Shortcut for reading a register
 */
static
inline
u_int32_t
emu_rreg(struct emu_softc *sc, u_int32_t reg)
{
	return bus_read_register(sc->e_busdata, sc->e_buspos, reg);
}

/*
 * Shortcut for writing a register
 */
static
inline
void
emu_wreg(struct emu_softc *sc, u_int32_t reg, u_int32_t val)
{
	bus_write_register(sc->e_busdata, sc->e_buspos, reg, val);
}

/*
 * Called by the underlying bus code when an interrupt happens
 */
void
emu_irq(void *dev)
{
	struct emu_softc *sc = dev;

	sc->e_result = emu_rreg(sc, REG_RESULT);
	emu_wreg(sc, REG_RESULT, 0);

	V(sc->e_sem);
}

/*
 * Convert the error codes reported by the "hardware" to errnos.
 * Or, on cases that indicate a programming error in emu.c, panic.
 */
static
u_int32_t
translate_err(struct emu_softc *sc, u_int32_t code)
{
	switch (code) {
	    case EMU_RES_SUCCESS: return 0;
	    case EMU_RES_BADHANDLE: 
	    case EMU_RES_BADOP: 
	    case EMU_RES_BADSIZE: 
		panic("emu%d: got fatal result code %d\n", sc->e_unit, code);
	    case EMU_RES_BADPATH: return ENOENT;
	    case EMU_RES_EXISTS: return EEXIST;
	    case EMU_RES_ISDIR: return EISDIR;
	    case EMU_RES_MEDIA: return EIO;
	    case EMU_RES_NOHANDLES: return ENFILE;
	    case EMU_RES_NOSPACE: return ENOSPC;
	    case EMU_RES_NOTDIR: return ENOTDIR;
	    case EMU_RES_UNKNOWN: return EIO;
	    case EMU_RES_UNSUPP: return EUNIMP;
	}
	kprintf("emu%d: Unknown result code %d\n", sc->e_unit, code);
	return EAGAIN;
}

/*
 * Wait for an operation to complete, and return an errno for the result.
 */
static
int
emu_waitdone(struct emu_softc *sc)
{
	P(sc->e_sem);
	return translate_err(sc, sc->e_result);
}

/*
 * Common file open routine (for both VOP_LOOKUP and VOP_CREATE).  Not
 * for VOP_OPEN. At the hardware level, we need to "open" files in
 * order to look at them, so by the time VOP_OPEN is called the
 * files are already open.
 */
static
int
emu_open(struct emu_softc *sc, u_int32_t handle, const char *name,
	 int create, int excl, u_int32_t *newhandle, int *newisdir)
{
	u_int32_t op;
	int result;

	if (strlen(name)+1 > EMU_MAXIO) {
		return ENAMETOOLONG;
	}

	if (create && excl) {
		op = EMU_OP_EXCLCREATE;
	}
	else if (create) {
		op = EMU_OP_CREATE;
	}
	else {
		op = EMU_OP_OPEN;
	}

	lock_acquire(sc->e_lock);

	strcpy(sc->e_iobuf, name);
	emu_wreg(sc, REG_IOLEN, strlen(name));
	emu_wreg(sc, REG_HANDLE, handle);
	emu_wreg(sc, REG_OPER, op);
	result = emu_waitdone(sc);

	if (result==0) {
		*newhandle = emu_rreg(sc, REG_HANDLE);
		*newisdir = emu_rreg(sc, REG_IOLEN)>0;
	}

	lock_release(sc->e_lock);
	return result;
}

/*
 * Routine for closing a file we opened at the hardware level.
 * This is not necessarily called at VOP_CLOSE time; it's called
 * at VOP_RECLAIM time.
 */
static
int
emu_close(struct emu_softc *sc, u_int32_t handle)
{
	int result;
	int mine;
	int retries=0;

	mine = lock_do_i_hold(sc->e_lock);
	if (!mine) {
		lock_acquire(sc->e_lock);
	}

	while (1) {
		/* Retry operation up to 10 times */

		emu_wreg(sc, REG_HANDLE, handle);
		emu_wreg(sc, REG_OPER, EMU_OP_CLOSE);
		result = emu_waitdone(sc);

		if (result==EIO && retries < 10) {
			kprintf("emu%d: I/O error on close, retrying\n", 
				sc->e_unit);
			retries++;
			continue;
		}
		break;
	}

	if (!mine) {
		lock_release(sc->e_lock);
	}
	return result;
}

/*
 * Common code for read and readdir.
 */
static
int
emu_doread(struct emu_softc *sc, u_int32_t handle, u_int32_t len,
	   u_int32_t op, struct uio *uio)
{
	int result;

	assert(uio->uio_rw == UIO_READ);

	lock_acquire(sc->e_lock);

	emu_wreg(sc, REG_HANDLE, handle);
	emu_wreg(sc, REG_IOLEN, len);
	emu_wreg(sc, REG_OFFSET, uio->uio_offset);
	emu_wreg(sc, REG_OPER, op);
	result = emu_waitdone(sc);
	if (result) {
		goto out;
	}
	
	result = uiomove(sc->e_iobuf, emu_rreg(sc, REG_IOLEN), uio);

	uio->uio_offset = emu_rreg(sc, REG_OFFSET);

 out:
	lock_release(sc->e_lock);
	return result;
}

/*
 * Read from a hardware-level file handle.
 */
static
int
emu_read(struct emu_softc *sc, u_int32_t handle, u_int32_t len,
	 struct uio *uio)
{
	return emu_doread(sc, handle, len, EMU_OP_READ, uio);
}

/*
 * Read a directory entry from a hardware-level file handle.
 */
static
int
emu_readdir(struct emu_softc *sc, u_int32_t handle, u_int32_t len,
	    struct uio *uio)
{
	return emu_doread(sc, handle, len, EMU_OP_READDIR, uio);
}

/*
 * Write to a hardware-level file handle.
 */
static
int
emu_write(struct emu_softc *sc, u_int32_t handle, u_int32_t len,
	  struct uio *uio)
{
	int result;

	assert(uio->uio_rw == UIO_WRITE);

	lock_acquire(sc->e_lock);

	emu_wreg(sc, REG_HANDLE, handle);
	emu_wreg(sc, REG_IOLEN, len);
	emu_wreg(sc, REG_OFFSET, uio->uio_offset);

	result = uiomove(sc->e_iobuf, len, uio);
	if (result) {
		goto out;
	}

	emu_wreg(sc, REG_OPER, EMU_OP_WRITE);
	result = emu_waitdone(sc);

 out:
	lock_release(sc->e_lock);
	return result;
}

/*
 * Get the file size associated with a hardware-level file handle.
 */
static
int
emu_getsize(struct emu_softc *sc, u_int32_t handle, off_t *retval)
{
	int result;

	lock_acquire(sc->e_lock);

	emu_wreg(sc, REG_HANDLE, handle);
	emu_wreg(sc, REG_OPER, EMU_OP_GETSIZE);
	result = emu_waitdone(sc);
	if (result==0) {
		*retval = emu_rreg(sc, REG_IOLEN);
	}

	lock_release(sc->e_lock);
	return result;
}

/*
 * Truncate a hardware-level file handle.
 */
static
int
emu_trunc(struct emu_softc *sc, u_int32_t handle, off_t len)
{
	int result;

	lock_acquire(sc->e_lock);

	emu_wreg(sc, REG_HANDLE, handle);
	emu_wreg(sc, REG_IOLEN, len);
	emu_wreg(sc, REG_OPER, EMU_OP_TRUNC);
	result = emu_waitdone(sc);

	lock_release(sc->e_lock);
	return result;
}

//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// vnode functions 
//

// at bottom of this section

static int emufs_loadvnode(struct emufs_fs *ef, u_int32_t handle, int isdir,
			   struct emufs_vnode **ret);

/*
 * VOP_OPEN on files
 */
static
int
emufs_open(struct vnode *v, int openflags)
{
	/*
	 * At this level we do not need to handle O_CREAT, O_EXCL, or O_TRUNC.
	 * We *would* need to handle O_APPEND, but we don't support it.
	 *
	 * Any of O_RDONLY, O_WRONLY, and O_RDWR are valid, so we don't need
	 * to check that either.
	 */

	if (openflags & O_APPEND) {
		return EUNIMP;
	}

	(void)v;

	return 0;
}

/*
 * VOP_OPEN on directories
 */
static
int
emufs_opendir(struct vnode *v, int openflags)
{
	switch (openflags & O_ACCMODE) {
	    case O_RDONLY:
		break;
	    case O_WRONLY:
	    case O_RDWR:
	    default:
		return EISDIR;
	}
	if (openflags & O_APPEND) {
		return EISDIR;
	}

	(void)v;
	return 0;
}

/*
 * VOP_CLOSE
 */
static
int
emufs_close(struct vnode *v)
{
	(void)v;
	return 0;
}

/*
 * VOP_RECLAIM
 *
 * Reclaim should make an effort to returning errors other than EBUSY.
 */
static
int
emufs_reclaim(struct vnode *v)
{
	struct emufs_vnode *ev = v->vn_data;
	struct emufs_fs *ef = v->vn_fs->fs_data;
	int ix, i, num, result;

	lock_acquire(ef->ef_emu->e_lock);
	lock_acquire(ev->ev_v.vn_countlock);

	if (ev->ev_v.vn_refcount != 1) {
		lock_release(ev->ev_v.vn_countlock);
		lock_release(ef->ef_emu->e_lock);
		return EBUSY;
	}

	/*
	 * Since we hold e_lock and are the last ref, nobody can increment
	 * the refcount, so we can release vn_countlock.
	 */
	lock_release(ev->ev_v.vn_countlock);

	/* emu_close retries on I/O error */
	result = emu_close(ev->ev_emu, ev->ev_handle);
	if (result) {
		lock_release(ef->ef_emu->e_lock);
		return result;
	}

	ix = -1;
	num = array_getnum(ef->ef_vnodes);
	for (i=0; i<num; i++) {
		struct emufs_vnode *evx = array_getguy(ef->ef_vnodes, i);
		if (evx==ev) {
			ix = i;
			break;
		}
	}
	if (ix<0) {
		panic("emu%d: reclaim vnode %u not in vnode pool\n",
		      ef->ef_emu->e_unit, ev->ev_handle);
	}
	array_remove(ef->ef_vnodes, ix);

	lock_release(ef->ef_emu->e_lock);

	VOP_KILL(&ev->ev_v);

	kfree(ev);
	return 0;
}

/*
 * VOP_READ
 */
static
int
emufs_read(struct vnode *v, struct uio *uio)
{
	struct emufs_vnode *ev = v->vn_data;
	u_int32_t amt;
	size_t oldresid;
	int result;

	assert(uio->uio_rw==UIO_READ);

	while (uio->uio_resid > 0) {
		amt = uio->uio_resid;
		if (amt > EMU_MAXIO) {
			amt = EMU_MAXIO;
		}

		oldresid = uio->uio_resid;

		result = emu_read(ev->ev_emu, ev->ev_handle, amt, uio);
		if (result) {
			return result;
		}
		
		if (uio->uio_resid == oldresid) {
			/* nothing read - EOF */
			break;
		}
	}

	return 0;
}

/*
 * VOP_READDIR
 */
static
int
emufs_getdirentry(struct vnode *v, struct uio *uio)
{
	struct emufs_vnode *ev = v->vn_data;
	u_int32_t amt;

	assert(uio->uio_rw==UIO_READ);

	amt = uio->uio_resid;
	if (amt > EMU_MAXIO) {
		amt = EMU_MAXIO;
	}

	return emu_readdir(ev->ev_emu, ev->ev_handle, amt, uio);
}

/*
 * VOP_WRITE
 */
static
int
emufs_write(struct vnode *v, struct uio *uio)
{
	struct emufs_vnode *ev = v->vn_data;
	u_int32_t amt;
	size_t oldresid;
	int result;

	assert(uio->uio_rw==UIO_WRITE);

	while (uio->uio_resid > 0) {
		amt = uio->uio_resid;
		if (amt > EMU_MAXIO) {
			amt = EMU_MAXIO;
		}

		oldresid = uio->uio_resid;

		result = emu_write(ev->ev_emu, ev->ev_handle, amt, uio);
		if (result) {
			return result;
		}

		if (uio->uio_resid == oldresid) {
			/* nothing written...? */
			break;
		}
	}

	return 0;
}

/*
 * VOP_IOCTL
 */
static
int
emufs_ioctl(struct vnode *v, int op, userptr_t data)
{
	/*
	 * No ioctls.
	 */

	(void)v;
	(void)op;
	(void)data;

	return EINVAL;
}

/*
 * VOP_STAT
 */
static
int
emufs_stat(struct vnode *v, struct stat *statbuf)
{
	struct emufs_vnode *ev = v->vn_data;
	int result;

	bzero(statbuf, sizeof(struct stat));

	result = VOP_GETTYPE(v, &statbuf->st_mode);
	if (result) {
		return result;
	}

	statbuf->st_nlink = 1;  /* might be a lie, but doesn't matter much */

	result = emu_getsize(ev->ev_emu, ev->ev_handle, &statbuf->st_size);
	if (result) {
		return result;
	}

	statbuf->st_blocks = 0;  /* almost certainly a lie */

	return 0;
}

/*
 * VOP_GETTYPE for files
 */
static
int
emufs_file_gettype(struct vnode *v, u_int32_t *result)
{
	(void)v;
	*result = S_IFREG;
	return 0;
}

/*
 * VOP_GETTYPE for directories
 */
static
int
emufs_dir_gettype(struct vnode *v, u_int32_t *result)
{
	(void)v;
	*result = S_IFDIR;
	return 0;
}

/*
 * VOP_TRYSEEK
 */
static
int
emufs_tryseek(struct vnode *v, off_t pos)
{
	if (pos<0) {
		return EINVAL;
	}

	/* Allow anything else */
	(void)v;

	return 0;
}

/*
 * VOP_FSYNC
 */
static
int
emufs_fsync(struct vnode *v)
{
	(void)v;
	return 0;
}

/*
 * VOP_TRUNCATE
 */
static
int
emufs_truncate(struct vnode *v, off_t len)
{
	struct emufs_vnode *ev = v->vn_data;
	return emu_trunc(ev->ev_emu, ev->ev_handle, len);
}

/*
 * VOP_CREAT
 */
static
int
emufs_creat(struct vnode *dir, const char *name, int excl, struct vnode **ret)
{
	struct emufs_vnode *ev = dir->vn_data;
	struct emufs_fs *ef = dir->vn_fs->fs_data;
	struct emufs_vnode *newguy;
	u_int32_t handle;
	int result;
	int isdir;

	result = emu_open(ev->ev_emu, ev->ev_handle, name, 1, excl, 
			  &handle, &isdir);
	if (result) {
		return result;
	}

	result = emufs_loadvnode(ef, handle, isdir, &newguy);
	if (result) {
		emu_close(ev->ev_emu, handle);
		return result;
	}

	*ret = &newguy->ev_v;
	return 0;
}

/*
 * VOP_LOOKUP
 */
static
int
emufs_lookup(struct vnode *dir, char *pathname, struct vnode **ret)
{
	struct emufs_vnode *ev = dir->vn_data;
	struct emufs_fs *ef = dir->vn_fs->fs_data;
	struct emufs_vnode *newguy;
	u_int32_t handle;
	int result;
	int isdir;

	result = emu_open(ev->ev_emu, ev->ev_handle, pathname, 0, 0,
			  &handle, &isdir);
	if (result) {
		return result;
	}

	result = emufs_loadvnode(ef, handle, isdir, &newguy);
	if (result) {
		emu_close(ev->ev_emu, handle);
		return result;
	}

	*ret = &newguy->ev_v;
	return 0;
}

/*
 * VOP_LOOKPARENT
 */
static
int
emufs_lookparent(struct vnode *dir, char *pathname, struct vnode **ret,
		 char *buf, size_t len)
{
	char *s;

	s = strrchr(pathname, '/');
	if (s==NULL) {
		/* just a last component, no directory part */
		if (strlen(pathname)+1 > len) {
			return ENAMETOOLONG;
		}
		VOP_INCREF(dir);
		*ret = dir;
		strcpy(buf, pathname);
		return 0;
	}

	*s = 0;
	s++;
	if (strlen(s)+1 > len) {
		return ENAMETOOLONG;
	}
	strcpy(buf, s);

	return emufs_lookup(dir, pathname, ret);
}

/*
 * VOP_NAMEFILE
 */
static
int
emufs_namefile(struct vnode *v, struct uio *uio)
{
	struct emufs_vnode *ev = v->vn_data;
	struct emufs_fs *ef = v->vn_fs->fs_data;

	if (ev == ef->ef_root) {
		/*
		 * Root directory - name is empty string
		 */
		return 0;
	}

	(void)uio;
	
	return EUNIMP;
}

//////////////////////////////

/*
 * Generic routines to return specific errors
 */

static
int
emufs_notdir(void)
{
	return ENOTDIR;
}

static
int
emufs_isdir(void)
{
	return EISDIR;
}

static
int
emufs_unimp(void)
{
	return EUNIMP;
}

/*
 * Casting through void * prevents warnings.
 * All of the vnode ops return int, and it's ok to cast functions that
 * take args to functions that take no args.
 */
#define ISDIR ((void *)emufs_isdir)
#define NOTDIR ((void *)emufs_notdir)
#define UNIMP ((void *)emufs_unimp)

/*
 * Function table for emufs files.
 */
static const struct vnode_ops emufs_fileops = {
	VOP_MAGIC,	/* mark this a valid vnode ops table */

	emufs_open,
	emufs_close,
	emufs_reclaim,

	emufs_read,
	NOTDIR,  /* readlink */
	NOTDIR,  /* getdirentry */
	emufs_write,
	emufs_ioctl,
	emufs_stat,
	emufs_file_gettype,
	emufs_tryseek,
	emufs_fsync,
	UNIMP,   /* mmap */
	emufs_truncate,
	NOTDIR,  /* namefile */

	NOTDIR,  /* creat */
	NOTDIR,  /* symlink */
	NOTDIR,  /* mkdir */
	NOTDIR,  /* link */
	NOTDIR,  /* remove */
	NOTDIR,  /* rmdir */
	NOTDIR,  /* rename */

	NOTDIR,  /* lookup */
	NOTDIR,  /* lookparent */
};

/*
 * Function table for emufs directories.
 */
static const struct vnode_ops emufs_dirops = {
	VOP_MAGIC,	/* mark this a valid vnode ops table */

	emufs_opendir,
	emufs_close,
	emufs_reclaim,

	ISDIR,   /* read */
	ISDIR,   /* readlink */
	emufs_getdirentry,
	ISDIR,   /* write */
	emufs_ioctl,
	emufs_stat,
	emufs_dir_gettype,
	UNIMP,   /* tryseek */
	ISDIR,   /* fsync */
	ISDIR,   /* mmap */
	ISDIR,   /* truncate */
	emufs_namefile,

	emufs_creat,
	UNIMP,   /* symlink */
	UNIMP,   /* mkdir */
	UNIMP,   /* link */
	UNIMP,   /* remove */
	UNIMP,   /* rmdir */
	UNIMP,   /* rename */

	emufs_lookup,
	emufs_lookparent,
};

/*
 * Function to load a vnode into memory.
 */
static
int
emufs_loadvnode(struct emufs_fs *ef, u_int32_t handle, int isdir,
		struct emufs_vnode **ret)
{
	struct emufs_vnode *ev;
	int i, num, result;

	lock_acquire(ef->ef_emu->e_lock);

	num = array_getnum(ef->ef_vnodes);
	for (i=0; i<num; i++) {
		ev = array_getguy(ef->ef_vnodes, i);
		if (ev->ev_handle==handle) {
			/* Found */

			VOP_INCREF(&ev->ev_v);

			lock_release(ef->ef_emu->e_lock);
			*ret = ev;
			return 0;
		}
	}

	/* Didn't have one; create it */

	ev = kmalloc(sizeof(struct emufs_vnode));
	if (ev==NULL) {
		lock_release(ef->ef_emu->e_lock);
		return ENOMEM;
	}

	ev->ev_emu = ef->ef_emu;
	ev->ev_handle = handle;

	result = VOP_INIT(&ev->ev_v, isdir ? &emufs_dirops : &emufs_fileops,
			   &ef->ef_fs, ev);
	if (result) {
		lock_release(ef->ef_emu->e_lock);
		kfree(ev);
		return result;
	}

	result = array_add(ef->ef_vnodes, ev);
	if (result) {
		lock_release(ef->ef_emu->e_lock);
		/* note: VOP_KILL undoes VOP_INIT - it does not kfree */
		VOP_KILL(&ev->ev_v);
		kfree(ev);
		return result;
	}

	lock_release(ef->ef_emu->e_lock);

	*ret = ev;
	return 0;
}

//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// filesystem functions 
//

/*
 * FSOP_SYNC
 */
static
int
emufs_sync(struct fs *fs)
{
	(void)fs;
	return 0;
}

/*
 * FSOP_GETVOLNAME
 */
static
const char *
emufs_getvolname(struct fs *fs)
{
	/* We don't have a volume name beyond the device name */
	(void)fs;
	return NULL;
}

/*
 * FSOP_GETROOT
 */
static
struct vnode *
emufs_getroot(struct fs *fs)
{
	struct emufs_fs *ef;

	assert(fs!=NULL);

	ef = fs->fs_data;

	assert(ef!=NULL);
	assert(ef->ef_root!=NULL);

	VOP_INCREF(&ef->ef_root->ev_v);
	return &ef->ef_root->ev_v;
}

/*
 * FSOP_UNMOUNT
 */
static
int
emufs_unmount(struct fs *fs)
{
	/* Always prohibit unmount, as we're not really "mounted" */
	(void)fs;
	return EBUSY;
}

/*
 * Routine for "mounting" an emufs - we're not really mounted in the
 * sense that the VFS understands that term, because we're not
 * connected to a block device.
 *
 * Basically, we just add ourselves to the name list in the VFS layer.
 */
static
int
emufs_addtovfs(struct emu_softc *sc, const char *devname)
{
	struct emufs_fs *ef;
	int result;

	ef = kmalloc(sizeof(struct emufs_fs));
	if (ef==NULL) {
		return ENOMEM;
	}

	ef->ef_fs.fs_sync = emufs_sync;
	ef->ef_fs.fs_getvolname = emufs_getvolname;
	ef->ef_fs.fs_getroot = emufs_getroot;
	ef->ef_fs.fs_unmount = emufs_unmount;
	ef->ef_fs.fs_data = ef;

	ef->ef_emu = sc;
	ef->ef_root = NULL;
	ef->ef_vnodes = array_create();
	if (ef->ef_vnodes == NULL) {
		kfree(ef);
		return ENOMEM;
	}

	result = emufs_loadvnode(ef, EMU_ROOTHANDLE, 1, &ef->ef_root);
	if (result) {
		kfree(ef);
		return result;
	}

	assert(ef->ef_root!=NULL);

	result = vfs_addfs(devname, &ef->ef_fs);
	if (result) {
		VOP_DECREF(&ef->ef_root->ev_v);
		kfree(ef);
	}
	return result;
}

//
////////////////////////////////////////////////////////////

/*
 * Config routine called by autoconf stuff.
 *
 * Initialize our data, then add ourselves to the VFS layer.
 *
 * Note that the config_*() functions are expected not to fail,
 * so we don't.
 */
int
config_emu(struct emu_softc *sc, int emuno)
{
	char name[32];

	sc->e_lock = lock_create("emufs-lock");
	if (sc->e_lock == NULL) {
		return ENOMEM;
	}
	sc->e_sem = sem_create("emufs-sem", 0);
	if (sc->e_sem == NULL) {
		lock_destroy(sc->e_lock);
		sc->e_lock = NULL;
		return ENOMEM;
	}
	sc->e_iobuf = bus_map_area(sc->e_busdata, sc->e_buspos, EMU_BUFFER);

	snprintf(name, sizeof(name), "emu%d", emuno);

	return emufs_addtovfs(sc, name);
}
