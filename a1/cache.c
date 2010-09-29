/*
 * CSC 369 Fall 2010 - Assignment 1
 *
 * $Id
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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
pthread_mutex_t slot_locks[NUM_SLOTS];

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

struct file_table {
  int size;
  struct bnode *head;
};

/* The global variable holding the file table */
struct file_table ftable[NUM_FILES];


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

    /* Initialize mutex for each slot */
    if(pthread_mutex_init(&slot_locks[i], NULL) != 0){
      fprintf(stderr, "Error Initializing Mutex\n");
      exit(1);
    }
  }
}

/* Simulates the read operation for the block block_num of file file_id, 
 * for the thread pid.
 * Returns 0 if the block was needed to be fetched from the disk, 
 *         1 if the block was found in the cache
 *         2 if the requested block was invalid
 */
int read_block(int pid, int file_id, int block_num) {
  /* Implement this and change the return value */
  return 0;
}

/* Simulates the write operation for the block block_num of file file_id, 
 * for the thread pid. It sets the dirty flag in the cache slot for the block.
 * Returns 0 if the block was needed to be fetched from the disk, 
 *         1 if the block was found in the cache
 *         2 if the requested block was invalid
 */
int write_block(int pid, int file_id, int block_num) {
  /* Implement this and change the return value */
  return 0;
}
