#ifndef _TEST_H_
#define _TEST_H_

/*
 * Declarations for test code and other miscellaneous functions.
 */

/* These are only actually available if OPT_SYNCHPROBS is set. */
int oneway_sem(int, char **);
int oneway_lock(int, char **);

/*
 * Test code.
 */

/* lib tests */
int arraytest(int, char **);
int bitmaptest(int, char **);
int queuetest(int, char **);

/* thread tests */
int threadtest(int, char **);
int threadtest2(int, char **);
int threadtest3(int, char **);
int semtest(int, char **);
int locktest(int, char **);
int cvtest(int, char **);
int jointest1(int, char **); // ASST1 test for thread_join
int jointest2(int, char **); // ASST1 test for thread_join

/* filesystem tests */
int fstest(int, char **);
int readstress(int, char **);
int writestress(int, char **);
int writestress2(int, char **);
int createstress(int, char **);
int printfile(int, char **);

/* other tests */
int malloctest(int, char **);
int mallocstress(int, char **);
int nettest(int, char **);

int coremaptest(int, char **);   // ASST2 basic coremap test
int coremapstress(int, char **); // ASST2 tougher coremap test

/* Kernel menu system */
void menu(char *argstr);

/* Routine for running userlevel test code. */
int runprogram(char *progname);

#endif /* _TEST_H_ */
