/*
 * CSC 369 Fall 2010 - Assignment 1
 *
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "rv.h"
#include "common.h"

struct slot {
  int file_id;
  unsigned int block_num;
  unsigned short dirty;
};

/* The global variable holding the cache structure */
struct slot cache[NUM_SLOTS];

/* array of mutexes for each slot */
pthread_mutex_t cache_locks[NUM_SLOTS];

struct bnode {
  int block_num;
  int cache_index;
  struct bnode *next;
};

typedef struct bnode bNode;

/* function returns pointer to node if found, else NULL */
bNode *bNode_search(bNode *head, int block_num){
  if(head == NULL)
    return NULL;

  bNode *curr = head;
  while(curr != NULL){
    if(curr->block_num == block_num)
      return curr;

    curr = curr->next;
  }

  return NULL;
}

/* add new node to front of list and return new head */
bNode *bNode_add(bNode *head, int block_num, int cache_index){
  bNode *temp = malloc(sizeof (bNode));

  /* Initialize node */
  temp->block_num = block_num;
  temp->cache_index = cache_index;

  temp->next = head;

  return temp;
}

/* remove node from list and return the head */
bNode *bNode_remove(bNode *head, int block_num){
  bNode *prev, *curr, *toRemove;
  int found = 0;

  prev = curr = head;
  while(curr != NULL){
    if(curr->block_num == block_num){
      found = 1;
      toRemove = curr;
      break;
    }

    prev = curr;
    curr = curr->next;
  }

  if(found == 0)
    return head;

  if(toRemove == head)
    head = head->next;
  else 
    prev->next = toRemove->next;

  free(toRemove);
  return head;
}

/* free up each node of the list and return NULL */
bNode *bNode_free_list(bNode *head){
  bNode *next = head;

  while(head != NULL){
    next = head->next;
    free(head);
    head = next;
  }

  return NULL;
}

struct file_table {
  int size;
  struct bnode *head;
};

/* The global variable holding the file table */
struct file_table ftable[NUM_FILES];

/* array of mutexes for each file */
pthread_mutex_t ftable_locks[NUM_FILES];

/* mutex for io request */
pthread_mutex_t io_lock;

/* variable to track empty slots along with mutex */
int slot_count = NUM_SLOTS;
pthread_mutex_t slot_count_lock;

/* return slot number if available else -1 */
int get_empty_slot(){
  pthread_mutex_lock(&slot_count_lock);

  int ret_val;
  if(slot_count > 0){
    ret_val = NUM_SLOTS - slot_count;
    slot_count--;
  } else{
    ret_val = -1;
  }

  pthread_mutex_unlock(&slot_count_lock);
  return ret_val;
}

/* Initialize the file table data structure with file sizes 
 * chosen from a Geometric distribution.
 */
void build_file_table() {
  int i;
  double p = 1 - (1.0 / MEAN_FILE_SIZE);
  for(i = 0; i < NUM_FILES; i++) {
    double k = Geometric(p);
    ftable[i].size = k + 1;  /* Files can't have size 0 */
    ftable[i].head = NULL;
  }
}

/* free up each list in the file table */
void destroy_file_table(){
  for(int i=0; i<NUM_FILES; i++)
    ftable[i].head = bNode_free_list(ftable[i].head);
}

/* Return the size of the file specified by fileid.
 */
int get_file_size(int fileid) {
  /* Since this function doesn't access the linked list structure,
   * no synchronization should be needed in it's logic
   */
  if((fileid < 0) || (fileid >= NUM_FILES))
    return 0;
  else
    return ftable[fileid].size;
}

void init_cache() {
  for(int i = 0; i < NUM_SLOTS; i++){
    /* Initialize each slot to free */
    cache[i].file_id = -1;
    cache[i].dirty = 0;

    /* Initialize mutex for each slot */
    if(pthread_mutex_init(&cache_locks[i], NULL) != 0){
      fprintf(stderr, "Error Initializing Mutex\n");
      exit(1);
    }
  }

  /* Initialize mutex for each file */
  for(int i = 0; i < NUM_FILES; i++){
    if(pthread_mutex_init(&ftable_locks[i], NULL) != 0){
      fprintf(stderr, "Error Initializing Mutex\n");
      exit(1);
    }
  }

  /* Initialize io lock */
  if(pthread_mutex_init(&io_lock, NULL) != 0){
    fprintf(stderr, "Error Initializing Mutex\n");
    exit(1);
  }

  /* Initialize slot_count lock */
  if(pthread_mutex_init(&slot_count_lock, NULL) != 0){
    fprintf(stderr, "Error Initializing Mutex\n");
    exit(1);
  }
}

/* create timespec with a given number of milliseconds */
void sleep_timespec(struct timespec *sleep_time, int msec){
  /* convert milli seconds to nano seconds */
  int nsec = msec*1000;
  /* tv_nsec cannot be greater than 999999999, thus need
   * to use seconds fields if extra time needed */
  if(nsec >= 1000000000){
    /* floor to nearest second, put rest in nanosec */
    sleep_time->tv_sec = floor(nsec/1000000000.0);
    sleep_time->tv_nsec = nsec - (sleep_time->tv_sec * 1000000000);
  } else {
    sleep_time->tv_sec = 0;
    sleep_time->tv_nsec = nsec;
  }
}

/* evict a block from the cache, writing it to disk first if dirty */
void evict_block(int slot){
  int file_id = cache[slot].file_id;
  int block_num = cache[slot].block_num;

  if(cache[slot].dirty == 1){
    /* copy block from cache to disk
     * i.e. sleep for DISK_TIME */
    struct timespec sleep_time, sleep_rem;
    sleep_timespec(&sleep_time, DISK_TIME);

    /* Only one thread can access disk at a time */
    pthread_mutex_lock(&io_lock);
    nanosleep(&sleep_time, &sleep_rem);
    pthread_mutex_unlock(&io_lock);

    /* Now mark slot as non-dirty */
    cache[slot].dirty = 0;
  } 

  /* remove block from owning file's list */
  pthread_mutex_lock(&ftable_locks[file_id]);
  ftable[file_id].head = bNode_remove(ftable[file_id].head, block_num);
  pthread_mutex_unlock(&ftable_locks[file_id]);

  /* mark slot as free */
  cache[slot].file_id = -1;
}

/* Simulates the read operation for the block block_num of file file_id, 
 * for the thread pid.
 * Returns 0 if the block was needed to be fetched from the disk, 
 *         1 if the block was found in the cache
 *         2 if the requested block was invalid
 */
int read_block(int pid, int file_id, int block_num) {
  /* check if file_id is valid */
  if((file_id < 0) || (file_id >= NUM_FILES)){
    return 2;
  }

  pthread_mutex_lock(&ftable_locks[file_id]);
  
  /* check if invalid request */
  if((block_num < 0) || (block_num >= get_file_size(file_id))){
    pthread_mutex_unlock(&ftable_locks[file_id]);

    return 2;
  } else if(bNode_search(ftable[file_id].head, block_num) != NULL){
    /* if block found in cache */
    pthread_mutex_unlock(&ftable_locks[file_id]);

    /* sleep for MEM_TIME */
    struct timespec sleep_time, sleep_rem;
    sleep_timespec(&sleep_time, MEM_TIME);

    nanosleep(&sleep_time, &sleep_rem);

    return 1;
  }

  /* unlock file since we're not going to modify it until
   * we've copied the block over to the cache */
  pthread_mutex_unlock(&ftable_locks[file_id]);

  /* get empty slot if available else randomly select one */
  int slot = get_empty_slot();
  if(slot == -1){
    slot = Equilikely(0, NUM_SLOTS-1);
  }

  pthread_mutex_lock(&cache_locks[slot]);

  /* if block not empty, evict it */
  if(cache[slot].file_id != -1){
    evict_block(slot);
  }

  /* copy block from disk to cache
   * i.e. sleep for DISK_TIME */
  struct timespec sleep_time, sleep_rem;
  sleep_timespec(&sleep_time, DISK_TIME);

  /* only one thread can access disk at a time */
  pthread_mutex_lock(&io_lock);
  nanosleep(&sleep_time, &sleep_rem);
  pthread_mutex_unlock(&io_lock);

  /* update the slot with block info */
  cache[slot].file_id = file_id;
  cache[slot].block_num = block_num;
  cache[slot].dirty = 0;

  /* now add the block info to file list */
  pthread_mutex_lock(&ftable_locks[file_id]);
  ftable[file_id].head = bNode_add(ftable[file_id].head, block_num, slot);
  pthread_mutex_unlock(&ftable_locks[file_id]);

  pthread_mutex_unlock(&cache_locks[slot]);

  return 0;
}

/* Simulates the write operation for the block block_num of file file_id, 
 * for the thread pid. It sets the dirty flag in the cache slot for the block.
 * Returns 0 if the block was needed to be fetched from the disk, 
 *         1 if the block was found in the cache
 *         2 if the requested block was invalid
 */
int write_block(int pid, int file_id, int block_num) {
  /* check if file_id i s valid */
  if((file_id < 0) || (file_id >= NUM_FILES)){
    return 2;
  }

  pthread_mutex_lock(&ftable_locks[file_id]);
  bNode *node = NULL;
  
  /* check if invalid request */
  if((block_num < 0) || (block_num >= get_file_size(file_id))){
    pthread_mutex_unlock(&ftable_locks[file_id]);

    return 2;
  } else if((node = bNode_search(ftable[file_id].head, block_num)) != NULL){
    /* if block found in cache */
    int slot = node->cache_index;
    pthread_mutex_unlock(&ftable_locks[file_id]);

    /* lock slot and write */
    pthread_mutex_lock(&cache_locks[slot]);

    /* sleep for MEM_TIME */
    struct timespec sleep_time, sleep_rem;
    sleep_timespec(&sleep_time, MEM_TIME);
    nanosleep(&sleep_time, &sleep_rem);

    /* update dirty flag */
    cache[slot].dirty = 1;

    pthread_mutex_unlock(&cache_locks[slot]);

    return 1;
  }

  pthread_mutex_unlock(&ftable_locks[file_id]);
  return 0;
}

int __main(int argc, char **argv){
  build_file_table();
  init_cache();

  /* read a couple of block from file 0 */
  read_block(0, 0, 1);
  read_block(0, 2, 1);
  read_block(0, 0, 2);
  read_block(0, 2, 2);
  read_block(0, 0, 3);
  read_block(0, 2, 3);
  read_block(0, 0, 4);
  read_block(0, 2, 4);
  read_block(0, 0, 5);
  read_block(0, 2, 5);
  read_block(0, 0, 6);
  read_block(0, 2, 6);
  read_block(0, 0, 7);
  read_block(0, 2, 7);
  read_block(0, 0, 8);
  read_block(0, 2, 8);
  read_block(0, 0, 9);
  read_block(0, 2, 9);
  read_block(0, 0, 10);
  read_block(0, 2, 10);

  read_block(0, 0, 0);

  for(int i=0; i < NUM_FILES; i++){
    ftable[i].head = bNode_free_list(ftable[i].head);
  }

  return 0;
}
