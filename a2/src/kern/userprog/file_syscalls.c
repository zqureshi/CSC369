/*
 * File-related system call implementations.
 * New for ASST1
 * Only crippled version of read/write for console I/O.
 */

#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <lib.h>
#include <uio.h>
#include <thread.h>
#include <curthread.h>
#include <vfs.h>
#include <vnode.h>
#include <syscall.h>


/* dumb_consoleIO_bootstrap
 * Initialize a vnode for the console device so that
 * user programs can use it.
 */
struct vnode *cons_vnode;

void dumb_consoleIO_bootstrap() 
{
  int result;
  char path[5];

  /* The path passed to vfs_open must be mutable. 
   * vfs_open may modify it.
   */
  
  cons_vnode = (struct vnode *)kmalloc(sizeof(struct vnode));
  if (cons_vnode == NULL) {
    kprintf("Warning: could not initialize console vnode\n");
    kprintf("User programs will not be able to read/write\n");
    return;
  }

  strcpy(path, "con:");
  result = vfs_open(path, O_RDWR, &cons_vnode);

  if (result) {
    /* Tough one... if there's no console, there's not
     * much point printing a warning...
     * but maybe the bootstrap was just called in the wrong place
     */
    kprintf("Warning: could not initialize console vnode\n");
    kprintf("User programs will not be able to read/write\n");
    kfree(cons_vnode);
    cons_vnode = NULL;
  }
}

/*
 * mk_useruio
 * sets up the uio for a USERSPACE transfer. 
 */
static
void
mk_useruio(struct uio *u, userptr_t buf, size_t len, off_t offset, 
	   enum uio_rw rw)
{
	assert(u);

	u->uio_iovec.iov_ubase = buf;
	u->uio_iovec.iov_len = len;
	u->uio_offset = offset;
	u->uio_resid = len;
	u->uio_segflg = UIO_USERSPACE;
	u->uio_rw = rw;
	u->uio_space = curthread->t_vmspace;
}


/*
 * sys_read
 * calls VOP_READ.
 */
int
sys_read(int fd, userptr_t buf, size_t size, int *retval)
{
	struct uio useruio;
	int result;
	int offset = 0;

	/* Make sure we were able to init the cons_vnode */
	if (cons_vnode == NULL) {
	  return ENODEV;
	}

	/* better be a valid file descriptor */
	/* Right now, only stdin (0), stdout (1) and stderr (2)
	 * are supported, and they can't be redirected to a file
	 */
	if (fd < 0 || fd > 2) {
	  return EBADF;
	}

	/* set up a uio with the buffer, its size, and the current offset */
	mk_useruio(&useruio, buf, size, offset, UIO_READ);

	/* does the read */
	result = VOP_READ(cons_vnode, &useruio);
	if (result) {
		return result;
	}

	/*
	 * The amount read is the size of the buffer originally, minus
	 * how much is left in it.
	 */
	*retval = size - useruio.uio_resid;

	return 0;
}

/*
 * sys_write
 * calls VOP_WRITE.
 */
int
sys_write(int fd, userptr_t buf, size_t size, int *retval)
{
	struct uio useruio;
	int result;
	int offset = 0;

	/* Make sure we were able to init the cons_vnode */
	if (cons_vnode == NULL) {
	  return ENODEV;
	}

	/* Right now, only stdin (0), stdout (1) and stderr (2)
	 * are supported, and they can't be redirected to a file
	 */
	if (fd < 0 || fd > 2) {
	  return EBADF;
	}

	/* set up a uio with the buffer, its size, and the current offset */
	mk_useruio(&useruio, buf, size, offset, UIO_WRITE);

	/* does the write */
	result = VOP_WRITE(cons_vnode, &useruio);
	if (result) {
		return result;
	}

	/*
	 * the amount written is the size of the buffer originally,
	 * minus how much is left in it.
	 */
	*retval = size - useruio.uio_resid;

	return 0;
}

