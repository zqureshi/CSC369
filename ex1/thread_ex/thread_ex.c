/* 
 * CSC 369 Fall 2010 - Excercise 1
 *
 * Zeeshan Qureshi
 * zeeshan.qureshi@utoronto.ca
 * $Id$
 */

#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>

/* Initialize mutexx for synchronizing threads */
pthread_mutex_t threadsCreated;

/* Function that each thread runs */
void *thread_func(void *tid){
  int id =  (int)tid;

  printf("Thread %d created\n", id);

  /* Wait for main program to signal creation of all threads */
  pthread_mutex_lock(&threadsCreated);
  printf("Thread %d exiting\n", id);
  pthread_mutex_unlock(&threadsCreated);

  pthread_exit(NULL);
}

int main(int argc, char **argv){
  /* Check if the argument has been passed or not */
  if(argc < 2){
    printf("Not enough arguments!!\n");
    return -1;
  }

  /* Parse the command line argument to an int */
  int numThreads = strtol(argv[1], NULL, 10);

  /* Initialize mutex and print message */
  pthread_mutex_init(&threadsCreated, NULL);
  pthread_mutex_lock(&threadsCreated);
  printf("Main thread: Beginning test\n");

  /* Allocate array for threads and initialize them */
  pthread_t *threads = malloc(sizeof(pthread_t) * numThreads);
  for(int  i=0; i<numThreads; i++){
    int rc = pthread_create(&threads[i], NULL, thread_func, (void *)i);
    if(rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      return -1;
    }
  }

  /* Sleep for some time to allow threads to be created */
  sleep(5);

  /* Unlock mutex and wait for threads to join */
  pthread_mutex_unlock(&threadsCreated);
  for(int i=0; i<numThreads; i++){
    pthread_join(threads[i], NULL);
  }

  /* Print final message, free up memory and exit */
  printf("Main thread: Test complete\n");
  pthread_mutex_destroy(&threadsCreated);
  free(threads);
  pthread_exit(NULL);
}
