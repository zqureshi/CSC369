#ifndef _UIO_H_
#define _UIO_H_

/*
 * Like BSD uio, but simplified a bit. (In BSD, there can be more than one
 * iovec in a uio.)
 */

enum uio_rw {
	UIO_READ,
	UIO_WRITE,
};

enum uio_seg {
	UIO_USERSPACE,
	UIO_SYSSPACE,
	UIO_USERISPACE,
};

struct iovec {
	union {
		void      *un_kbase;   /* kernel address (UIO_SYSSPACE) */
		userptr_t  un_ubase;   /* user address (UIO_USER{,I}SPACE */
	} iov_un;
	size_t iov_len;                /* Length of data */
};
#define iov_kbase  iov_un.un_kbase
#define iov_ubase  iov_un.un_ubase

struct uio {
	struct iovec      uio_iovec;       /* Data block */
	off_t             uio_offset;      /* desired offset into object */
	size_t            uio_resid;       /* Remaining amt of data to xfer */
	enum uio_seg      uio_segflg;      /* what kind of pointer we have */
	enum uio_rw       uio_rw;          /* whether op is a read or write */
	struct addrspace *uio_space;       /* address space for user pointer */
};


/*
 * Copy data from a kernel buffer to a data region defined by a uio struct,
 * updating the uio struct's offset and resid fields. May alter the iovec
 * fields as well.
 *
 * Before calling this, you should
 *   (1) set up uio_iovec to point to the buffer you want to transfer to;
 *   (2) initialize uio_offset as desired;
 *   (3) initialize uio_resid to the total amount of data that can be 
 *       transferred through this uio;
 *   (4) set up uio_seg and uio_rw correctly;
 *   (5) if uio_seg is UIO_SYSSPACE, set uio_space to NULL; otherwise,
 *       initialize uio_space to the address space in which the buffer
 *       should be found.
 *
 * After calling, 
 *   (1) the contents of uio_iovec may be altered and should not be 
 *       interpreted;
 *   (2) uio_offset will have been incremented by the amount transferred;
 *   (3) uio_resid will have been decremented by the amount transferred;
 *   (4) uio_segflg, uio_rw, and uio_space will be unchanged.
 *
 * uiomove() may be called repeatedly on the same uio to transfer
 * additional data until the available buffer space the uio refers to
 * is exhausted.
 *
 * Note that the actual value of uio_offset is not interpreted. It is
 * provided to allow for easier file seek pointer management.
 *
 * When uiomove is called, the address space presently in context must
 * be the same as the one recorded in uio_space. This is an important
 * sanity check if I/O has been queued.
 */

int uiomove(void *kbuffer, size_t len, struct uio *uio);

/*
 * Like uiomove, but sends zeros.
 */
int uiomovezeros(size_t len, struct uio *uio);

/*
 * Initialize uio for I/O from a kernel buffer.
 */
void mk_kuio(struct uio *, void *kbuf, size_t len, off_t pos, enum uio_rw rw);

#endif /* _UIO_H_ */
