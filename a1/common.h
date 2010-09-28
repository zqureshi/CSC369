/*
 * CSC 369 Fall 2010 - Assignment 1
 *
 * $Id
 */

#define MEAN_FILE_SIZE 20
#define NUM_FILES 5         /* The number of files in the file table */
#define NUM_SLOTS 20        /* The number of slots in the cache array */
#define NUM_PROCESSES 10    /* The number of threads in the simulation */

#define READ_PROB 0.9       /* The probability that a block request will be
                               a read request */

/* All time constants are given in milliseconds 
 */
#define MEM_TIME 1          /* The time to read a block from the cache */
#define DISK_TIME 4000      /* The time to transfer a block from disk to cache */

#define MIN_COMPUTE_TIME 10
#define MAX_COMPUTE_TIME 99

int get_file_size(int fileid);
void build_file_table();
void init_cache();

int read_block(int pid, int id, int blocknum);
int write_block(int pid, int id, int blocknum);
