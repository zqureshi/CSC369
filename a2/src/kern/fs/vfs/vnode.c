/*
 * Basic vnode support functions.
 */
#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <synch.h>
#include <vnode.h>

/*
 * Initialize an abstract vnode.
 * Invoked by VOP_INIT.
 */
int
vnode_init(struct vnode *vn, const struct vnode_ops *ops,
	   struct fs *fs, void *fsdata)
{
	assert(vn!=NULL);
	assert(ops!=NULL);

	vn->vn_ops = ops;
	vn->vn_refcount = 1;
	vn->vn_opencount = 0;
	vn->vn_countlock = lock_create("vnode-countlock");
	if (vn->vn_countlock == NULL) {
		return ENOMEM;
	}
	vn->vn_fs = fs;
	vn->vn_data = fsdata;
	return 0;
}

/*
 * Destroy an abstract vnode.
 * Invoked by VOP_KILL.
 */
void
vnode_kill(struct vnode *vn)
{
	assert(vn->vn_refcount==1);
	assert(vn->vn_opencount==0);
	assert(vn->vn_countlock!=NULL);

	lock_destroy(vn->vn_countlock);

	vn->vn_ops = NULL;
	vn->vn_refcount = 0;
	vn->vn_opencount = 0;
	vn->vn_countlock = NULL;
	vn->vn_fs = NULL;
	vn->vn_data = NULL;
}


/*
 * Increment refcount.
 * Called by VOP_INCREF.
 */
void
vnode_incref(struct vnode *vn)
{
	assert(vn!=NULL);
	lock_acquire(vn->vn_countlock);
	vn->vn_refcount++;
	lock_release(vn->vn_countlock);
}

/*
 * Decrement refcount.
 * Called by VOP_DECREF.
 * Calls VOP_RECLAIM if the refcount hits zero.
 */
void
vnode_decref(struct vnode *vn)
{
	int result, actually_do_it = 0;

	assert(vn!=NULL);

	lock_acquire(vn->vn_countlock);
	assert(vn->vn_refcount>0);
	if (vn->vn_refcount>1) {
		vn->vn_refcount--;
	}
	else {
		actually_do_it = 1;
	}
	lock_release(vn->vn_countlock);

	if (actually_do_it) {
		result = VOP_RECLAIM(vn);
		if (result != 0 && result != EBUSY) {
			// XXX: lame.
			kprintf("vfs: Warning: VOP_RECLAIM: %s\n",
				strerror(result));
		}
	}
}

/*
 * Increment the open count.
 * Called by VOP_INCOPEN.
 */
void
vnode_incopen(struct vnode *vn)
{
	assert(vn!=NULL);
	lock_acquire(vn->vn_countlock);
	vn->vn_opencount++;
	lock_release(vn->vn_countlock);
}

/*
 * Decrement the open count.
 * Called by VOP_DECOPEN.
 */
void
vnode_decopen(struct vnode *vn)
{
	int opencount, result;

	assert(vn!=NULL);
	lock_acquire(vn->vn_countlock);
	assert(vn->vn_opencount>0);
	vn->vn_opencount--;
	opencount = vn->vn_opencount;
	lock_release(vn->vn_countlock);

	if (opencount > 0) {
		return;
	}

	result = VOP_CLOSE(vn);
	if (result) {
		// XXX: also lame.
		// The FS should do what it can to make sure this code
		// doesn't get reached...
		kprintf("vfs: Warning: VOP_CLOSE: %s\n", strerror(result));
	}
}

/*
 * Check for various things being valid.
 * Called before all VOP_* calls.
 */
void
vnode_check(struct vnode *v, const char *opstr)
{
	if (v == NULL) {
		panic("vnode_check: vop_%s: null vnode\n", opstr);
	}
	if (v == (void *)0xdeadbeef) {
		panic("vnode_check: vop_%s: deadbeef vnode\n", opstr);
	}

	if (v->vn_ops == NULL) {
		panic("vnode_check: vop_%s: null ops pointer\n", opstr);
	}
	if (v->vn_ops == (void *)0xdeadbeef) {
		panic("vnode_check: vop_%s: deadbeef ops pointer\n", opstr);
	}

	if (v->vn_ops->vop_magic != VOP_MAGIC) {
		panic("vnode_check: vop_%s: ops with bad magic number %lx\n", 
		      opstr, v->vn_ops->vop_magic);
	}

	// Device vnodes have null fs pointers.
	//if (v->vn_fs == NULL) {
	//	panic("vnode_check: vop_%s: null fs pointer\n", opstr);
	//}
	if (v->vn_fs == (void *)0xdeadbeef) {
		panic("vnode_check: vop_%s: deadbeef fs pointer\n", opstr);
	}

	lock_acquire(v->vn_countlock);

	if (v->vn_refcount < 0) {
		panic("vnode_check: vop_%s: negative refcount %d\n", opstr,
		      v->vn_refcount);
	}
	else if (v->vn_refcount == 0 && strcmp(opstr, "reclaim")) {
		panic("vnode_check: vop_%s: zero refcount\n", opstr);
	}
	else if (v->vn_refcount > 0x100000) {
		kprintf("vnode_check: vop_%s: warning: large refcount %d\n", 
			opstr, v->vn_refcount);
	}

	if (v->vn_opencount < 0) {
		panic("vnode_check: vop_%s: negative opencount %d\n", opstr,
		      v->vn_opencount);
	}
	else if (v->vn_opencount > 0x100000) {
		kprintf("vnode_check: vop_%s: warning: large opencount %d\n", 
			opstr, v->vn_opencount);
	}

	lock_release(v->vn_countlock);
}
