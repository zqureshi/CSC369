#ifndef _DEV_H_
#define _DEV_H_

struct uio;  /* in <uio.h> */

/*
 * Filesystem-namespace-accessible device.
 * d_io is for both reads and writes; the uio indicates which should be done.
 */
struct device {
	int (*d_open)(struct device *, int flags_from_open);
	int (*d_close)(struct device *);
	int (*d_io)(struct device *, struct uio *);
	int (*d_ioctl)(struct device *, int op, userptr_t data);

	u_int32_t d_blocks;
	u_int32_t d_blocksize;

	void *d_data;   /* device-specific data */
};

/* Create vnode for namespace-accessible device. */
struct vnode *dev_create_vnode(struct device *dev);


/* Builtin namespace-accessible devices. */
void devnull_create(void);


/*
 * Device probe functions.
 *
 *      dev_bootstrap - start machine-independent device initialization.
 *
 * The corresponding machine-dependent function is called by the
 * machine-independent function.
 */

void dev_bootstrap(void);

void machdep_dev_bootstrap(void);

#endif /* _DEV_H_ */
