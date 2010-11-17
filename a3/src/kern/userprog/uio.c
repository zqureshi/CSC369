#include <types.h>
#include <lib.h>
#include <uio.h>
#include <thread.h>
#include <curthread.h>

/*
 * See uio.h for a description.
 */

int
uiomove(void *ptr, size_t n, struct uio *uio)
{
	struct iovec *iov;
	size_t size;
	int result;

	if (uio->uio_rw != UIO_READ && uio->uio_rw != UIO_WRITE) {
		panic("uiomove: Invalid uio_rw %d\n", (int) uio->uio_rw);
	}
	if (uio->uio_segflg==UIO_SYSSPACE) {
		assert(uio->uio_space == NULL);
	}
	else {
		assert(uio->uio_space == curthread->t_vmspace);
	}

	while (n > 0 && uio->uio_resid > 0) {
		iov = &uio->uio_iovec;
		size = iov->iov_len;

		if (size > n) {
			size = n;
		}

		if (size==0) {
			/* 
			 * This should only happen if you set uio_resid
			 * incorrectly (to more than the total length of
			 * buffers the uio points to). 
			 */
			panic("uiomove: size reached 0\n");
		}

		switch (uio->uio_segflg) {
		    case UIO_SYSSPACE:
			    result = 0;
			    if (uio->uio_rw == UIO_READ) {
				    memmove(iov->iov_kbase, ptr, size);
			    }
			    else {
				    memmove(ptr, iov->iov_kbase, size);
			    }
			    iov->iov_kbase = ((char *)iov->iov_kbase+size);
			    break;
		    case UIO_USERSPACE:
		    case UIO_USERISPACE:
			    if (uio->uio_rw == UIO_READ) {
				    result = copyout(ptr, iov->iov_ubase,size);
			    }
			    else {
				    result = copyin(iov->iov_ubase, ptr, size);
			    }
			    if (result) {
				    return result;
			    }
			    iov->iov_ubase += size;
			    break;
		    default:
			    panic("uiomove: Invalid uio_segflg %d\n",
				  (int)uio->uio_segflg);
		}

		iov->iov_len -= size;
		uio->uio_resid -= size;
		uio->uio_offset += size;
		ptr = ((char *)ptr + size);
		n -= size;
	}

	return 0;
}

int
uiomovezeros(size_t n, struct uio *uio)
{
	/* static, so initialized as zero */
	static char zeros[16];
	size_t amt;
	int result;

	/* This only makes sense when reading */
	assert(uio->uio_rw == UIO_READ);

	while (n>0) {
		amt = sizeof(zeros);
		if (amt > n) {
			amt = n;
		}
		result = uiomove(zeros, amt, uio);
		if (result) {
			return result;
		}
		n -= amt;
	}

	return 0;
}

/*
 * Convenience function to cons up a uio for kernel I/O.
 */
void
mk_kuio(struct uio *uio, void *kbuf, size_t len, off_t pos, enum uio_rw rw)
{
	uio->uio_iovec.iov_kbase = kbuf;
	uio->uio_iovec.iov_len = len;
	uio->uio_offset = pos;
	uio->uio_resid = len;
	uio->uio_segflg = UIO_SYSSPACE;
	uio->uio_rw = rw;
	uio->uio_space = NULL;
}
