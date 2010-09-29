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

struct bnode {
  int block_num;
  int cache_index;
  struct bnode *next;
};

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
  /* Implement this and change the return value */
  return 0;
}

void init_cache() {
  /* Implement this */
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
