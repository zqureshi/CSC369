#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/queue.h>
#include <sys/time.h>

#include "binheap.h"

static void
bh_test(unsigned algo, unsigned psz, unsigned ntest)
{
	unsigned u, ux, ul;

	bh_init(algo, psz);
	for (u = 0; u < ntest; u++)
		bh_insert(random());
	for (u = 0; u < ntest; u++) {
		bh_remove();
		bh_insert(random());
	}
	ul = 0;
	for (u = 0; u < ntest; u++) {
		ux = bh_remove();
		assert(ul <= ux);
		ul = ux;
	}
}

static void
test0(unsigned algo, unsigned ntest, unsigned psz)
{
	unsigned apg, step;
	unsigned npgu, npo;

	srandom(0);
	npgu = -1;
	step = 1;
	for (apg = 1; apg <= npgu; apg += step) {
		if (apg > 100)
			step = 10;
		if (apg > 1000)
			step = 100;
		if (npgu - apg < 100)
			step = 1;
		VM_init(apg, psz);
		bh_test(algo, psz, ntest);
		VM_finish(&npgu, &npo);
		printf("%u %u %u %u %u\n", algo, ntest, npgu - apg, npgu, npo);
	}
}

static unsigned
test1a(unsigned algo, unsigned apg, unsigned ntest, unsigned psz)
{
	unsigned npgu, npo;

	VM_init(apg, psz);
	srandom(0);
	bh_test(algo, psz, ntest);
	VM_finish(&npgu, &npo);
	printf(" - %u %u %u", algo, npgu, npo);
	return (npgu);
}


static void
test1(unsigned algo1, unsigned algo2, unsigned ntest, unsigned psz)
{
	unsigned a, apg, step;
	unsigned npgu, npgu1;

	npgu = 2;
	step = 1;
	for (apg = 1; apg <= npgu; apg += step) {

		printf("%u %u ", ntest, apg);

		for (a = algo1; a <= algo2; a++) {
			npgu1 = test1a(a, apg, ntest, psz);
			if (npgu1 > npgu)
				npgu = npgu1;
		}
		printf("\n");

		if (apg <= 100 || npgu - apg <= 100)
			step = 1;
		else if (apg <= 1000 || npgu - apg <= 1000)
			step = 10;
		else
			step = 100;
	}
}

static void
test2(unsigned algo, unsigned ntest, unsigned psz)
{
	unsigned npgu, npo;
	struct timespec t0, t1;
	double a;

	VM_init(-1, psz);
	srandom(0);
	bh_test(algo, psz, ntest);
	VM_finish(&npgu, &npo);
	clock_gettime(CLOCK_MONOTONIC, &t0);
	VM_init(-1, psz);
	srandom(0);
	bh_test(algo, psz, ntest);
	VM_finish(&npgu, &npo);
	clock_gettime(CLOCK_MONOTONIC, &t1);
	a = t1.tv_sec - t0.tv_sec;
	a += (t1.tv_nsec - t0.tv_nsec) * 1e-9;
	printf("# alg%d %g\n", algo, a);
}


/**********************************************************************
 */

int
main(int argc, char **argv)
{
	unsigned ntest = 50000;
	unsigned psize = 512;
	unsigned i, j;

	(void)argc;	
	(void)argv;

	if (argc > 1)
		ntest=strtoul(argv[1], NULL, 0);

	setbuf(stdout, NULL);

	if (0) {
		test0(0, ntest, psize);
		printf("\n\n");
		test0(1, ntest, psize);
		printf("\n\n");
		test0(2, ntest, psize);
		printf("\n\n");
		test0(3, ntest, psize);
	}
#define LOW_ALG 0
#define HIGH_ALG 3
	for (i = LOW_ALG; i <= HIGH_ALG; i++)
		for (j = 0; j < 5; j++)
			test2(i, ntest, psize);
	test1(LOW_ALG, HIGH_ALG, ntest, psize);
	return(0);
}
