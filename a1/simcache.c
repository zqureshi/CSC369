/*
 * CSC 369 Fall 2010 - Assignment 1
 *
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include "common.h"
#include "rv.h"

void compute(int min_time, int max_time) {
	long sleep_time = Equilikely(min_time, max_time);
	struct timespec t;
	t.tv_sec = 0;
	t.tv_nsec = sleep_time * 1000000;
	if(nanosleep(&t, NULL) == -1) {
		perror("nanosleep");
	}
}


void *process(void *arg) {
	int pid = (long)arg;

	// choose a file 
	int fileid = random() % NUM_FILES;
	int size = get_file_size(fileid);
	printf("[%d] starting, file %d, size %d\n", pid, fileid, size);

	// access each block of the file sequentially
	int i;
	for(i = 0; i < size; i++) {
		// processing time
		compute(MIN_COMPUTE_TIME, MAX_COMPUTE_TIME);
		// do the file read or write
		if(random() / (double)INT32_MAX < READ_PROB) {
			read_block(pid, fileid, i);
		} else {
			write_block(pid, fileid, i);
		}
	}

	printf("[%d] terminating\n", pid);
	return 0;
}

int _main(int argc, char **argv){
  /* Initialize all structures */
  build_file_table();
  init_cache();

  /* print size of each file */
  for(int i = 0; i < NUM_FILES; i++){
    printf("File Number: %d, Size: %d\n", i, get_file_size(i));
  }

  return 0;
}
