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

/* array to store read/write stats */
int io_stats[NUM_PROCESSES][3];

void *process(void *arg) {
	int pid = (long)arg;

	// choose a file 
	int fileid = random() % NUM_FILES;
	int size = get_file_size(fileid);
	printf("[%d] starting, file %d, size %d\n", pid, fileid, size);

  // initialize IO statistics
  io_stats[pid][0] = io_stats[pid][1] = io_stats[pid][2] = 0; 

	// access each block of the file sequentially
	int i;
	for(i = 0; i < size; i++) {
		// processing time
		compute(MIN_COMPUTE_TIME, MAX_COMPUTE_TIME);
		// do the file read or write
		if(random() / (double)INT32_MAX < READ_PROB) {
			io_stats[pid][read_block(pid, fileid, i)]++;
		} else {
			io_stats[pid][write_block(pid, fileid, i)]++;
		}
	}

	printf("[%d] terminating\n", pid);
  pthread_exit(NULL);
}

/* declare function to free up lists */
void destroy_file_table();

int main(int argc, char **argv){
  /* Initialize all structures */
  build_file_table();
  init_cache();

  pthread_t threads[NUM_PROCESSES];

  for(int i=0; i<NUM_PROCESSES; i++){
    if(pthread_create(&threads[i], NULL, process, (void *)i) != 0){
      fprintf(stderr, "Error creating thread\n");
      exit(1);
    }
  }

  void *status;
  for(int i=0; i<NUM_PROCESSES; i++){
    pthread_join(threads[i], &status);
  }

  double ratio, total_hits=0, total=0;
  printf("\nStatistics:\n");
  for(int i=0; i<NUM_PROCESSES; i++){
    ratio = (double)(io_stats[i][1])/
      (io_stats[i][0] + io_stats[i][1] + io_stats[i][2]);

    total_hits += io_stats[i][1];
    total += io_stats[i][0] + io_stats[i][1] + io_stats[i][2];

    printf("Thread %d, hits: %lf%% (%d)\n",
        i, ratio*100, io_stats[i][1]);
  }
  printf("Total hits: %lf%%\n", (double)total_hits/total*100);

  destroy_file_table();
  pthread_exit(NULL);
}
