
#include <types.h>
#include <lib.h>
#include <kern/unistd.h>
#include <kern/errno.h>
#include <kern/stat.h>
#include <vfs.h>
#include <vnode.h>
#include <uio.h>
#include <synch.h>
#include <cache.h>

struct cache *the_cache;
int max_block_id;


static 
int 
write_out_block(int id, void *data)
{
  struct uio buf_uio;
  int result;
  

  mk_kuio(&buf_uio, data, BUFSIZE, BUFSIZE*id, UIO_WRITE);

  result = VOP_WRITE(the_cache->backing_store, &buf_uio);

  if (result) {
    return result;
  }

  return 0;
}



static 
int 
read_in_block(int id, void *data)
{
  struct uio buf_uio;
  int result;
  

  mk_kuio(&buf_uio, data, BUFSIZE, BUFSIZE*id, UIO_READ);

  result = VOP_READ(the_cache->backing_store, &buf_uio);

  if (result) {
    return result;
  }

  return 0;
}


int cache_read(int id, void *blk)
{

  int i = 0;
  int old_id;
  int result;
  struct buf_hdr *victim = NULL;

  lock_acquire(the_cache->c_lock);

  /* Is the block we want already in the cache? */
  
  while (i < NBUFS) {
    for (i=0; i < NBUFS; i++) {
      
      if (the_cache->bufs[i].id == id) {
	/* Found in cache! Copy and return. */
	memcpy(blk, the_cache->bufs[i].data, BUFSIZE);
	lock_release(the_cache->c_lock);
	return 0;
      }
      
      if ( the_cache->bufs[i].replacing_id == id 
	   && the_cache->bufs[i].doing_io ) {
	DEBUG(DB_CACHE,"have to wait, reading block %d\n",id);
	cv_wait(the_cache->bufs[i].busy_cv, the_cache->c_lock);
	/* While we were waiting to get the lock again, the block we
	 * wanted could have been evicted, and read into a different 
	 * buffer.  Best start over from the top.
	 */
	break;
      }
    }
  }

  /* Not found in cache.  Select victim */

  /* A buffer that is being read in or written out should not be 
   * chosen as a victim.  If all buffers are being read/written,
   * we wait.  On return from wait, we need to rescan to find a
   * non-busy victim, if any.
   */
  int have_victim = FALSE;
  int first_idx = the_cache->next_idx;

  while (!have_victim) {
    victim = &the_cache->bufs[the_cache->next_idx];
    the_cache->next_idx = (the_cache->next_idx+1) % NBUFS;

    if (!victim->doing_io) {
      have_victim = TRUE;
    } else {
      if (the_cache->next_idx == first_idx) {
	/* We checked all the buffers, and they were ALL busy */
	DEBUG(DB_CACHE,"Have to wait to get a non-busy victim for block %d\n",id);
	cv_wait(the_cache->all_busy_cv,the_cache->c_lock);
	first_idx = the_cache->next_idx;
      }
    }
  }

  old_id = victim->id;
  victim->replacing_id = id; /* Record new id to hold off other readers */
  victim->doing_io = TRUE;
      
  if (victim->dirty) {

    assert(lock_do_i_hold(the_cache->c_lock));
    lock_release(the_cache->c_lock);
    result = write_out_block(old_id, victim->data);
    lock_acquire(the_cache->c_lock);

    assert(victim->id == old_id);
    assert(victim->doing_io);
    assert(victim->dirty);
    assert(victim->replacing_id == id);

    if (result) {
      /* Failed write shouldn't destroy buffer content.  
       * Keep original id, clear replacing_id, and return error.
       */
      victim->doing_io = FALSE;
      victim->replacing_id = -1;
      lock_release(the_cache->c_lock);
      return result;
    }

    victim->dirty = FALSE;
  }

  /* Now that buffer is clean, read the block we want into the cache. 
   * During the read, neither the old id (block being replaced) or
   * the new id (block being read in) is valid, so we set victim->id
   * to -1 during the read.
   */

  victim->id = -1;

  assert(lock_do_i_hold(the_cache->c_lock));
  lock_release(the_cache->c_lock);
  result = read_in_block(victim->replacing_id, victim->data);
  lock_acquire(the_cache->c_lock);

  assert(victim->id == -1);
  assert(victim->doing_io);
  assert(!victim->dirty);

  victim->doing_io = FALSE;
  victim->replacing_id = -1;

  if (result) {
    lock_release(the_cache->c_lock);
    return result;
  }

  victim->id = id;

  /* Copy from cache buffer to block provided */

  memcpy(blk, victim->data, BUFSIZE);

  /* Broadcast to waiters that wanted the same block, if any.
   * All of them can use the block now, so wake them all up. 
   */

  cv_broadcast(victim->busy_cv, the_cache->c_lock);

  /* Signal waiters that wanted a non-busy buffer, if any 
   * At most one buffer became non-busy, so no point waking up
   * more than one waiter of this type.
   */
  cv_signal(the_cache->all_busy_cv, the_cache->c_lock);

  lock_release(the_cache->c_lock);

  return 0;
}



int cache_write(int id, void *blk)
{

  /* If block "id" is already in cache, copy "blk" 
   * into cache buffer, otherwise just write it out 
   * to disk.
   */

  int i = 0;
  
  lock_acquire(the_cache->c_lock);

  while (i < NBUFS) {
    for (i=0; i < NBUFS; i++) {
      if (the_cache->bufs[i].id == id && !the_cache->bufs[i].doing_io) {
	/* Found it! */
	memcpy(the_cache->bufs[i].data, blk, BUFSIZE);
	the_cache->bufs[i].dirty = TRUE;
	lock_release(the_cache->c_lock);
	return 0;
      }

      if ( (the_cache->bufs[i].id == id || 
	    the_cache->bufs[i].replacing_id == id)
	   && the_cache->bufs[i].doing_io) {
	/* Wait for I/O to finish */
	DEBUG(DB_CACHE,"Have to wait on write for block %d\n",id);
	cv_wait(the_cache->bufs[i].busy_cv, the_cache->c_lock);
	/* Much could have changed while we were waiting.  Best
	 * start over from the top.
	 */
	break;
      }
    }
  }

  /* Not in cache.  Write straight to disk. */
  lock_release(the_cache->c_lock);

  return write_out_block(id, blk);

  
}



static void free_cache_mem(int lastalloc)
{
  int i;
  for (i=0; i < lastalloc; i++) {
    kfree(the_cache->bufs[i].data);
    if (the_cache->bufs[i].busy_cv) {
      cv_destroy(the_cache->bufs[i].busy_cv);
    }
  }

  if (the_cache->c_lock) {
    lock_destroy(the_cache->c_lock);
  }

  if (the_cache->all_busy_cv) {
    cv_destroy(the_cache->all_busy_cv);
  }

  kfree(the_cache);
  the_cache = NULL;

}


void check_cache_integrity()
{
  int i;
  int j;

  lock_acquire(the_cache->c_lock);
  DEBUG(DB_CACHE,"Checking cache...\n");
  for (i = 0; i < NBUFS; i++) {
    if (the_cache->bufs[i].id != -1) {
      for (j = i+1; j < NBUFS; j++) {
	if (the_cache->bufs[i].id == the_cache->bufs[j].id) {
	  kprintf("ERROR: cache integrity check fails. Buffer %d and buffer %d both have id %d\n",i,j,the_cache->bufs[i].id);
	}
      }
    }
  }

  lock_release(the_cache->c_lock);
}


int cache_init()
{
  int i;
  int result;
  char path[20];
  struct vnode *diskvn;
  struct stat diskstat;
  int sz;
  int diskbufs;
  struct buf_hdr *bufs;

  the_cache = (struct cache *)kmalloc(sizeof(struct cache));
  if (!the_cache) {
    return ENOMEM;
  }

  the_cache->c_lock = lock_create("Cache lock");
  if (!the_cache->c_lock) {
    kfree(the_cache);
    return ENOMEM;
  }

  the_cache->all_busy_cv = cv_create("Cache cv");
  if (!the_cache->all_busy_cv) {
    lock_destroy(the_cache->c_lock);
    kfree(the_cache);
    return ENOMEM;
  }

 
  bufs = the_cache->bufs;

  for (i=0; i< NBUFS; i++) {
    bufs[i].id = -1;
    bufs[i].dirty = FALSE;
    bufs[i].doing_io = FALSE;
    bufs[i].replacing_id = -1;
    bufs[i].busy_cv = cv_create("Buffer cv");
    if (!bufs[i].busy_cv) {
      free_cache_mem(i);
      return ENOMEM;
    }
    bufs[i].data = (char *)kmalloc(BUFSIZE);
    if (!bufs[i].data) {
      free_cache_mem(i);
      return ENOMEM;
    }
  }


  /* Initialize disk device to use for backing storage of buffers */

  /* Open raw disk device */
  strcpy(path,"lhd0raw:");
  result = vfs_open(path, O_RDWR, &diskvn);
  if (result) {
    free_cache_mem(NBUFS);
    return result;
  }

  /* Get size of disk to calculate how many buffers disk can store */

  result = VOP_STAT(diskvn, &diskstat);
  if (result) {
    vfs_close(diskvn);
    free_cache_mem(NBUFS);
    return result;
  }

  sz = diskstat.st_size;

  /* How many data buffers can we store on disk? */
  diskbufs = sz / BUFSIZE;

  max_block_id = diskbufs - 1;
  the_cache->backing_store = diskvn;


  return 0;

}

