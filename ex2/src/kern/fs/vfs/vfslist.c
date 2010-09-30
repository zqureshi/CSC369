/*
 * VFS operations that involve the list of VFS (named) devices
 * (the "dev" in "dev:path" syntax).
 */

#include <types.h>
#include <lib.h>
#include <synch.h>
#include <array.h>
#include <kern/errno.h>
#include <vfs.h>
#include <vnode.h>
#include <fs.h>
#include <dev.h>

/*
 * Structure for a single named device.
 * 
 * kd_name    - Name of device (eg, "lhd0"). Should always be set to
 *              a valid string.
 *
 * kd_rawname - Name of raw device (eg, "lhd0raw"). Is non-NULL if and
 *              only if this device can have a filesystem mounted on
 *              it.
 *
 * kd_device  - Device object this name refers to. May be NULL if kd_fs
 *              is hardwired.
 *
 * kd_fs      - Filesystem object mounted on, or associated with, this
 *              device. NULL if there is no filesystem. 
 *
 * A filesystem can be associated with a device without having been
 * mounted if the device was created that way. In this case,
 * kd_rawname is NULL (prohibiting mount/unmount), and, as there is
 * then no way to access kd_device, it will be NULL as well. This is
 * intended for devices that are inherently filesystems, like emu0.
 *
 * Referencing kd_name, or the filesystem volume name, on a device
 * with a filesystem mounted returns the root of the filesystem.
 * Referencing kd_name on a mountable device with no filesystem
 * returns ENXIO. Referencing kd_name on a device that is not
 * mountable and has no filesystem, or kd_rawname on a mountable
 * device, returns the device itself.
 */

struct knowndev {
	char *kd_name;
	char *kd_rawname;
	struct device *kd_device;
	struct vnode *kd_vnode;
	struct fs *kd_fs;
};

static struct array *knowndevs;
static struct lock *knowndevs_lock;

/*
 * Setup function
 */
void
vfs_bootstrap(void)
{
	knowndevs = array_create();
	if (knowndevs==NULL) {
		panic("vfs: Could not create knowndevs array\n");
	}
	knowndevs_lock = lock_create("knowndevs");
	if (knowndevs_lock==NULL) {
		panic("vfs: Could not create knowndevs lock\n");
	}

	vfs_initbootfs();
	devnull_create();
}

/*
 * Global sync function - call FSOP_SYNC on all devices.
 */
int
vfs_sync(void)
{
	struct knowndev *dev;
	int i, num;

	lock_acquire(knowndevs_lock);

	num = array_getnum(knowndevs);
	for (i=0; i<num; i++) {
		dev = array_getguy(knowndevs, i);
		if (dev->kd_fs != NULL) {
			/*result =*/ FSOP_SYNC(dev->kd_fs);
		}
	}

	lock_release(knowndevs_lock);

	return 0;
}

/*
 * Given a device name (lhd0, emu0, somevolname, null, etc.), hand
 * back an appropriate vnode.
 */
int
vfs_getroot(const char *devname, struct vnode **result)
{
	struct knowndev *kd;
	int i, num;
	int err=0;

	lock_acquire(knowndevs_lock);

	num = array_getnum(knowndevs);
	for (i=0; i<num; i++) {
		kd = array_getguy(knowndevs, i);

		/*
		 * If this device has a mounted filesystem, and we
		 * just named the filesystem, or just named the
		 * device, return the root of the filesystem.
		 *
		 * If it has no mounted filesystem, it's mountable,
		 * and we just named the device, return ENXIO.
		 */

		if (kd->kd_fs!=NULL) {
			const char *volname;
			volname = FSOP_GETVOLNAME(kd->kd_fs);

			if (!strcmp(kd->kd_name, devname) ||
			    (volname!=NULL && !strcmp(volname, devname))) {
				*result = FSOP_GETROOT(kd->kd_fs);
				goto out;
			}
		}
		else {
			if (kd->kd_rawname!=NULL &&
			    !strcmp(kd->kd_name, devname)) {
				err = ENXIO;
				goto out;
			}
		}

		/*
		 * If we named the device, and we got here, it must
		 * have no fs and not be mountable. In this case, we
		 * return the device itself.
		 */
		if (!strcmp(kd->kd_name, devname)) {
			assert(kd->kd_fs==NULL);
			assert(kd->kd_rawname==NULL);
			assert(kd->kd_device != NULL);
			VOP_INCREF(kd->kd_vnode);
			*result = kd->kd_vnode;
			goto out;
		}

		/*
		 * If the device has a rawname and we just named that,
		 * return the device itself.
		 */
		if (kd->kd_rawname!=NULL && !strcmp(kd->kd_rawname, devname)) {
			assert(kd->kd_device != NULL);
			VOP_INCREF(kd->kd_vnode);
			*result = kd->kd_vnode;
			goto out;
		}

		/*
		 * If none of the above tests matched, we didn't name
		 * any of the names of this device, so go on to the
		 * next one. 
		 */
	}

	/*
	 * If we got here, the device specified by devname doesn't exist.
	 */
	err = ENODEV;

 out:
	lock_release(knowndevs_lock);

	return err;
}

/*
 * Given a filesystem, hand back the name of the device it's mounted on.
 */
const char *
vfs_getdevname(struct fs *fs)
{
	struct knowndev *kd;
	int i, num;

	assert(fs != NULL);

	lock_acquire(knowndevs_lock);

	num = array_getnum(knowndevs);
	for (i=0; i<num; i++) {
		kd = array_getguy(knowndevs, i);

		if (kd->kd_fs == fs) {
			lock_release(knowndevs_lock);
			/*
			 * This is not a race condition: as long as the
			 * guy calling us holds a reference to the fs,
			 * the fs cannot go away, and the device can't
			 * go away until the fs goes away.
			 */
			return kd->kd_name;
		}
	}

	lock_release(knowndevs_lock);

	return NULL;
}

/*
 * Assemble the name for a raw device from the name for the regular device.
 */
static
char *
mkrawname(const char *name)
{
	char *s = kmalloc(strlen(name)+3+1);
	if (!s) {
		return NULL;
	}
	strcpy(s, name);
	strcat(s, "raw");
	return s;
}


/*
 * Check if the two strings passed in are the same, if they're both
 * not NULL (the latter part being significant).
 */
static
inline
int
samestring(const char *a, const char *b)
{
	if (a==NULL || b==NULL) {
		return 0;
	}
	return !strcmp(a, b);
}

/*
 * Check if the first string passed is the same as any of the three others,
 * if they're not NULL.
 */
static
inline
int
samestring3(const char *a, const char *b, const char *c, const char *d)
{
	return samestring(a,b) || samestring(a,c) || samestring(a,d);
}

/*
 * Check if any of the three names passed in already exists as a device
 * name.
 */

static
int
badnames(const char *n1, const char *n2, const char *n3)
{
	const char *volname;
	int i, num;
	struct knowndev *kd;

	assert(lock_do_i_hold(knowndevs_lock));

	num = array_getnum(knowndevs);
	for (i=0; i<num; i++) {
		kd = array_getguy(knowndevs, i);

		if (kd->kd_fs) {
			volname = FSOP_GETVOLNAME(kd->kd_fs);
			if (samestring3(volname, n1, n2, n3)) {
				return 1;
			}
		}

		if (samestring3(kd->kd_rawname, n1, n2, n3) ||
		    samestring3(kd->kd_name, n1, n2, n3)) {
			return 1;
		}
	}

	return 0;
}

/*
 * Add a new device to the VFS layer's device table.
 *
 * If "mountable" is set, the device will be treated as one that expects
 * to have a filesystem mounted on it, and a raw device will be created
 * for direct access.
 */
static
int
vfs_doadd(const char *dname, int mountable, struct device *dev, struct fs *fs)
{
	char *name=NULL, *rawname=NULL;
	struct knowndev *kd=NULL;
	struct vnode *vnode=NULL;
	const char *volname=NULL;
	int err=0;

	name = kstrdup(dname);
	if (name==NULL) {
		goto puke;
	}
	if (mountable) {
		rawname = mkrawname(name);
		if (rawname==NULL) {
			goto puke;
		}
	}

	vnode = dev_create_vnode(dev);
	if (vnode==NULL) {
		goto puke;
	}

	kd = kmalloc(sizeof(struct knowndev));
	if (kd==NULL) {
		goto puke;
	}

	kd->kd_name = name;
	kd->kd_rawname = rawname;
	kd->kd_device = dev;
	kd->kd_vnode = vnode;
	kd->kd_fs = fs;

	if (fs!=NULL) {
		volname = FSOP_GETVOLNAME(fs);
	}

	lock_acquire(knowndevs_lock);

	if (!badnames(name, rawname, volname)) {
		err = array_add(knowndevs, kd);
	}
	else {
		err = EEXIST;
	}

	lock_release(knowndevs_lock);

	return err;

 puke:

	if (name) {
		kfree(name);
	}
	if (rawname) {
		kfree(rawname);
	}
	if (vnode) {
		kfree(vnode);
	}
	if (kd) {
		kfree(kd);
	}
	
	return ENOMEM;
}

/*
 * Add a new device, by name. See above for the description of
 * mountable.
 */
int
vfs_adddev(const char *devname, struct device *dev, int mountable)
{
	return vfs_doadd(devname, mountable, dev, NULL);
}

/*
 * Add a filesystem that does not have an underlying device.
 * This is used for emufs, but might also be used for network
 * filesystems and the like.
 */
int
vfs_addfs(const char *devname, struct fs *fs)
{
	return vfs_doadd(devname, 0, NULL, fs);
}

//////////////////////////////////////////////////

/*
 * Look for a mountable device named DEVNAME.
 * Should already hold knowndevs_lock.
 */
static
int
findmount(const char *devname, struct knowndev **result)
{
	struct knowndev *dev;
	int i, num, found=0;

	assert(lock_do_i_hold(knowndevs_lock));

	num = array_getnum(knowndevs);
	for (i=0; !found && i<num; i++) {
		dev = array_getguy(knowndevs, i);
		if (dev->kd_rawname==NULL) {
			/* not mountable/unmountable */
			continue;
		}

		if (!strcmp(devname, dev->kd_name)) {
			*result = dev;
			found = 1;
		}
	}

	return found ? 0 : ENODEV;
}

/*
 * Mount a filesystem. Once we've found the device, call MOUNTFUNC to
 * set up the filesystem and hand back a struct fs. This is done with
 * the devices table locked.
 *
 * The DATA argument is passed through unchanged to MOUNTFUNC.
 */
int
vfs_mount(const char *devname, void *data,
	  int (*mountfunc)(void *data, struct device *, struct fs **ret))
{
	const char *volname;
	struct knowndev *kd;
	struct fs *fs;
	int result;

	lock_acquire(knowndevs_lock);
	

	result = findmount(devname, &kd);
	if (result) {
		goto puke;
	}

	if (kd->kd_fs != NULL) {
		result = EBUSY;
		goto puke;
	}
	assert(kd->kd_rawname != NULL);
	assert(kd->kd_device != NULL);

	result = mountfunc(data, kd->kd_device, &fs);
	if (result) {
		goto puke;
	}

	assert(fs!=NULL);

	kd->kd_fs = fs;

	volname = FSOP_GETVOLNAME(fs);
	kprintf("vfs: Mounted %s: on %s\n",
		volname ? volname : kd->kd_name, kd->kd_name);

	assert(result==0);
	
 puke:
	lock_release(knowndevs_lock);
	return result;
}

/*
 * Unmount a filesystem/device by name.
 * First calls FSOP_SYNC on the filesystem; then calls FSOP_UNMOUNT.
 */
int
vfs_unmount(const char *devname)
{
	struct knowndev *kd;
	int result;

	lock_acquire(knowndevs_lock);
	

	result = findmount(devname, &kd);
	if (result) {
		goto puke;
	}

	if (kd->kd_fs == NULL) {
		result = EINVAL;
		goto puke;
	}
	assert(kd->kd_rawname != NULL);
	assert(kd->kd_device != NULL);

	result = FSOP_SYNC(kd->kd_fs);
	if (result) {
		goto puke;
	}

	result = FSOP_UNMOUNT(kd->kd_fs);
	if (result) {
		goto puke;
	}

	kprintf("vfs: Unmounted %s:\n", kd->kd_name);

	/* now drop the filesystem */
	kd->kd_fs = NULL;

	assert(result==0);

 puke:
	lock_release(knowndevs_lock);
	return result;
}

/*
 * Global unmount function.
 */
int
vfs_unmountall(void)
{
	struct knowndev *dev;
	int i, num, result;

	lock_acquire(knowndevs_lock);

	num = array_getnum(knowndevs);
	for (i=0; i<num; i++) {
		dev = array_getguy(knowndevs, i);
		if (dev->kd_rawname == NULL) {
			/* not mountable/unmountable */
			continue;
		}
		if (dev->kd_fs == NULL) {
			/* not mounted */
			continue;
		}

		kprintf("vfs: Unmounting %s:\n", dev->kd_name);

		result = FSOP_SYNC(dev->kd_fs);
		if (result) {
			kprintf("vfs: Warning: sync failed for %s: %s, trying "
				"again\n", dev->kd_name, strerror(result));

			result = FSOP_SYNC(dev->kd_fs);
			if (result) {
				kprintf("vfs: Warning: sync failed second time"
					" for %s: %s, giving up...\n",
					dev->kd_name, strerror(result));
				continue;
			}
		}

		result = FSOP_UNMOUNT(dev->kd_fs);
		if (result==EBUSY) {
			kprintf("vfs: Cannot unmount %s: (busy)\n", 
				dev->kd_name);
			continue;
		}
		if (result) {
			kprintf("vfs: Warning: unmount failed for %s:"
				" %s, already synced, dropping...\n",
				dev->kd_name, strerror(result));
			continue;
		}

		/* now drop the filesystem */
		dev->kd_fs = NULL;
	}

	lock_release(knowndevs_lock);

	return 0;
}
