#ifndef _QUEUE_H_
#define _QUEUE_H_

/* 
 * Queue of void pointers, implemented as a ring buffer.
 *
 * Functions:
 *       q_create  - allocate a new queue, with initial size SIZE.
 *                   Returns NULL on error.
 *       q_preallocate - extend the queue if necessary so its size is
 *                   at least SIZE. Returns an error code.
 *       q_empty   - return true if queue is empty.
 *       q_addtail - add a pointer to the tail of the queue. If the queue
 *                   overflows, it will extend itself, but this is slow.
 *                   Returns an error code if the extension fails. Does not
 *                   fail if no extension is required.
 *       q_remhead - remove a pointer from the head of the queue. If the
 *                   queue is empty, panics.
 *       q_destroy - dispose of the queue.
 */

struct queue; /* Opaque. */

struct queue *q_create(int size);
int           q_preallocate(struct queue *, int size);
int           q_empty(struct queue *);
int           q_addtail(struct queue *, void *ptr);
void         *q_remhead(struct queue *);
void          q_destroy(struct queue *);

/* 
 * These are really intended only for debugging. Using them encodes 
 * knowledge of how the queue works, which is usually undesirable.
 *
 *      q_getstart - return the index of the front of the queue
 *      q_getend   - return the index of the back of the queue
 *      q_getsize  - return the current size of the queue
 *      q_getguy   - return queue member by index
 *
 * To iterate over the queue, do something like
 *      struct queue *q;
 *      int i;
 *      
 *      for (i=q_getstart(q); i!=q_getend(q); i=(i+1)%q_getsize(q)) {
 *              void *ptr = q_getguy(q, i);
 *                :
 *      }
 *
 * If you do this, synchronization is your problem.
 */
int   q_getstart(struct queue *);
int   q_getend(struct queue *);
int   q_getsize(struct queue *);
void *q_getguy(struct queue *, int index);

#endif /* _QUEUE_H_ */
