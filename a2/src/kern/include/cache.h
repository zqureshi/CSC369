
/* Number of buffers cache can hold in memory */
#define NBUFS 10

/* size of a single buffer's data area, in bytes */
/* Must be a multiple of the disk block size. */
#define BUFSIZE 512  

/* Each buffer is described by a "buffer header" which
 * includes a pointer to the data block for the buffer,
 * and an identifier that tells us where to write the 
 * block when it is moved out to disk (and where to 
 * read it back from!)
 *
 * You will want additional fields to manage the synchronization
 * requirements.
 */

struct buf_hdr {
  int id;        /* identifier == disk block # cached in this buffer */
  char *data;    /* content of buffer */
  int dirty;     /* TRUE if buffer data modified since read from disk */
 
  /* Add any other per-buffer state you need here */

  int doing_io;  /* TRUE if buf data is being read from or written to disk */
  int replacing_id;   /* Id of block that will replace current one */
  struct cv *busy_cv; /* wait for buffer io to complete */
};

/* The cache data structure itself records information
 * about the backing storage area (specifically, a pointer
 * to its vnode), an array of buffer headers, and the
 * index of the next buffer to use when a new one is needed.
 *
 * We implement a simple round-robin replacement as an
 * example.  There is an obvious enhancement you should 
 * add when you provide support for concurrent access to
 * the cache.
 * 
 * You will want additional fields to manage the synchronization
 * requirements.
 */

struct cache {
  struct vnode *backing_store;
  struct buf_hdr bufs[NBUFS];
  int next_idx;

  /* Add any other cache-wide state you need here */

  struct lock *c_lock;    /* cache-wide lock to synchronize access */
  struct cv *all_busy_cv; /* wait for victim if all buffers are doing io */
};

/* Read a block (identified by "id"), from the cache
 * by copying the buffer's data area into "blk".
 * Do NOT return a pointer to a block in the cache!
 */

extern int cache_read(int id, void *blk);

/* Write the contents of "blk" to the block
 * identified by "id".  If "id" is in the cache,
 * copy from "blk" to the data area of the buffer
 * that is caching "id".  Otherwise, write "blk"
 * to the backing store area for "id" (the entire
 * block is over-written so it does not need to 
 * be read in first in this case.)
 */

extern int cache_write(int id, void *blk);

/* Initialize the cache data structure.
 * Be sure to add code to initialize any new
 * fields you add for synchronization.
 */
  
extern int cache_init();

/* For users of the cache, the 
 * maximum block id that can be stored
 * on the backing storage device.
 * "id" > max_block_id cannot be cached
 * because we have nowhere to put it
 * if it can't be kept in memory.
 */

extern int max_block_id;

/* For testing purposes...
 * Locks the cache, and checks for duplicate block ids
 */

extern void check_cache_integrity();
