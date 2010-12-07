/*
 * File-related system call implementations.
 * New for ASST1
 * Only crippled version of read/write for console I/O.
 * New skeletons for A3 system call handlers added.
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
#include <kern/limits.h>
#include <kern/stat.h>
#include <synch.h>
#include <file.h>


/* dumb_consoleIO_bootstrap
 * Initialize a vnode for the console device so that
 * user programs can use it.
 * ASST3:
 *
 * This function is currently called during the boot 
 * sequence.  Initially, you can leave it in place so that
 * you will be able to run test programs that use stdin,
 * stdout and stderr file descriptors before you have the file
 * table support fully implemented.  This could be 
 * useful for incremental testing and development.
 *
 * This should be unnecessary once the console device is 
 * correctly attached to the first 3 file descriptors
 * in the per-process file table.
 *
 * The work of opening the console device and obtaining
 * a vnode will be the same, however instead of a single
 * global pointer to the console vnode, each process 
 * should have a pointer to an openfile instance in its
 * file table.
 * 
 * When you have that support implemented, remove the call
 * to this function from boot().
 */
struct vnode *cons_vnode;

void dumb_consoleIO_bootstrap() 
{
  int result;
  char path[5];

  /* The path passed to vfs_open must be mutable. 
   * vfs_open may modify it.
   */
  
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
 * sys_open
 * just copies in the filename, then passes work to file_open.
 * You have to write file_open.
 * 
 */
int
sys_open(userptr_t filename, int flags, int mode, int *retval)
{
	char fname[PATH_MAX];
	int result;

	result = copyinstr(filename, fname, sizeof(fname), NULL);
	if (result) {
		return result;
	}

	return file_open(fname, flags, mode, retval);
}

/*
 * sys_read
 * calls VOP_READ.
 * 
 * ASST3: This is the "dumb" implementation of sys_read:
 * it only deals with file descriptors 0,1 and 2, and 
 * assumes they are permanently associated with the 
 * console vnode (which is only initialized if you added 
 * a call to dumb_consoleIO_bootstrap).
 *
 * In your implementation, you should use the file descriptor
 * to obtain an openfile object from the file table, and
 * then read from the vnode recorded in that openfile.
 *
 * Note that any problems with the address supplied by the
 * user as "buf" will be handled by the VOP_READ / uio code
 * so you do not have to try to verify "buf" yourself.
 *
 * Most of this code should be replaced.
 */
int
sys_read(int fd, userptr_t buf, size_t size, int *retval)
{
	int result;
  struct openfile *of;
  
  /* Verify descriptor and find file in table */
  result = filetable_findfile(fd, &of);
  if(result){
    return result;
  }

  /* populate uio with offset from open file */
	struct uio useruio;
	int offset = of->of_offset;

	/* set up a uio with the buffer, its size, and the current offset */
	mk_useruio(&useruio, buf, size, offset, UIO_READ);

	/* does the read */
	result = VOP_READ(of->of_vnode, &useruio);
	if (result) {
		return result;
	}

  /* update offset in open file */
  of->of_offset = useruio.uio_offset;

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
 *
 * ASST3: This is the "dumb" implementation of sys_write:
 * it only deals with file descriptors 0,1 and 2, and 
 * assumes they are permanently associated with the 
 * console vnode (which is only initialized if you added 
 * a call to dumb_consoleIO_bootstrap).
 *
 * In your implementation, you should use the file descriptor
 * to obtain an openfile object from the file table, and
 * then read from the vnode recorded in that openfile.
 *
 * Note that any problems with the address supplied by the
 * user as "buf" will be handled by the VOP_READ / uio code
 * so you do not have to try to verify "buf" yourself.
 *
 * Most of this code should be replaced.
 */
int
sys_write(int fd, userptr_t buf, size_t size, int *retval)
{
	int result;
  struct openfile *of;
  
  /* Verify descriptor and find file in table */
  result = filetable_findfile(fd, &of);
  if(result){
    return result;
  }

  /* populate uio with offset from open file */
	struct uio useruio;
	int offset = of->of_offset;

	/* set up a uio with the buffer, its size, and the current offset */
	mk_useruio(&useruio, buf, size, offset, UIO_WRITE);

	/* does the write */
	result = VOP_WRITE(of->of_vnode, &useruio);
	if (result) {
		return result;
	}

  /* update offset in open file */
  of->of_offset = useruio.uio_offset;

	/*
	 * the amount written is the size of the buffer originally,
	 * minus how much is left in it.
	 */
	*retval = size - useruio.uio_resid;

	return 0;
}

/* 
 * sys_close
 * You have to write file_close.
 */
int
sys_close(int fd)
{
	return file_close(fd);
}

/*
 * sys_lseek
 * 
 */
int
sys_lseek(int fd, off_t offset, int whence, off_t *retval)
{
  int result;
  struct openfile *of;

  result = filetable_findfile(fd, &of);
  if(result){
    return result;
  }

  off_t pos;
  struct stat statbuf;
  switch(whence){
    case SEEK_SET:
      pos = offset;
      break;

    case SEEK_CUR:
      pos = of->of_offset + offset;
      break;

    case SEEK_END:
      VOP_STAT(of->of_vnode, &statbuf);
      pos = statbuf.st_size - 1 + offset;
      break;

    default:
      return EINVAL;
  }

  /* check pos */
  if(pos < 0){
    return EINVAL;
  }

  result = VOP_TRYSEEK(of->of_vnode, pos);
  if(result){
    return result;
  }

  /* If everything ok, modify fd offset */
  of->of_offset = pos;

  *retval = pos;
	return 0;
}

/* 
 * sys_dup2
 * 
 */
int
sys_dup2(int oldfd, int newfd, int *retval)
{
  int result;
  struct openfile *of;

  /* verify oldfd */
  result = filetable_findfile(oldfd, &of);
  if(result){
    return result;
  }

  /* verify newfd */
  if(newfd < 0 || newfd >= FOPEN_MAX){
    return EBADF;
  }

  /* close file at newfd if present */
  if(curthread->t_filetable->ft_openfiles[newfd] != NULL){
    file_close(newfd);
  }

  /* finally set newfd to point to oldfd and increase refcount */
  curthread->t_filetable->ft_openfiles[newfd] = of;
  of->of_refcount += 1;

  *retval = newfd;
  return 0;
}

/* really not "file" calls, per se, but might as well put it here */

/*
 * sys_chdir
 * 
 */
int
sys_chdir(userptr_t path)
{
        (void)path;

	return EUNIMP;
}

/*
 * sys___getcwd
 * 
 */
int
sys___getcwd(userptr_t buf, size_t buflen, int *retval)
{
        (void)buf;
        (void)buflen;
        (void)retval;

	return EUNIMP;
}

/*
 * sys_mkdir
 */
int
sys_mkdir(userptr_t path, int mode)
{
        (void)path;
        (void)mode;

	return EUNIMP;
}

/*
 * sys_rmdir
 */
int
sys_rmdir(userptr_t path)
{
        (void)path;

	return EUNIMP;
}

/*
 * sys_remove
 */
int
sys_remove(userptr_t path)
{
        (void)path;

	return EUNIMP;
}

/*
 * sys_rename
 * 
 * CAUTION: in sys_open, space for the kernel copy of the path name is 
 * allocated on the stack.  Trying to do the same thing here for both
 * oldpath and newpath is likely to cause trouble with stack overflow
 * (2*PATH_MAX == 2048 == half the total kernel stack).  Use kmalloc
 * instead.
 */
int
sys_rename(userptr_t oldpath, userptr_t newpath)
{
        (void)oldpath;
        (void)newpath;

	return EUNIMP;
}

/*
 * sys_getdirentry
 */
int
sys_getdirentry(int fd, userptr_t buf, size_t buflen, int *retval)
{
        (void)fd;
        (void)buf;
	(void)buflen;
        (void)retval;

	return EUNIMP;
}

/*
 * sys_fstat
 */
int
sys_fstat(int fd, userptr_t statptr)
{
        (void)fd;
        (void)statptr;

	return EUNIMP;
}
