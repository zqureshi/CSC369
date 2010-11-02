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

/* BEGIN A1 EXTRA */

/* basic buffer cache synchronization test - you should add more */
int buffertest1(int, char **);
int buffertest2(int, char **);

/* basic fork/join/exit tests */
int jointest1(int, char **);
int jointest2(int, char **);

/* END A1 EXTRA   */

/* Kernel menu system */
void menu(char *argstr);

/* Routine for running userlevel test code. */
int runprogram(char *progname, char **args);

#endif /* _TEST_H_ */
