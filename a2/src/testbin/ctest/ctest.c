/*
 * Performance test from former 161 prof. Brad Chen
 * Stresses VM.
 *
 * Intended for the VM assignment. This should run successfully on a
 * variety of strides when the VM system is complete. Strides that are
 * not a multiple of 2 work better; see below.
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * SIZE is the amount of memory used.
 * DEFAULT is the default stride.
 * Note that SIZE and DEFAULT should be relatively prime.
 */
#define SIZE      (1024*1024/sizeof(struct entry))
#define DEFAULT   477

struct entry {
	struct entry *e;
};

struct entry array[SIZE];

int
main(int argc, char **argv)
{
	volatile struct entry *e;
	unsigned i, stride;

	stride = DEFAULT;
	if (argc == 2) {
		stride = atoi(argv[1]);
	}
	if (stride <= 0 || argc > 2) {
		printf("Usage: ctest [stridesize]\n");
		printf("   stridesize should not be a multiple of 2.\n");
		return 1;
	}

	printf("Starting ctest: stride %d\n", stride);
	
	/*
	 * Generate a huge linked list, with each entry pointing to
	 * the slot STRIDE entries above it. As long as STRIDE and SIZE
	 * are relatively prime, this will put all the entries on one
	 * list. Otherwise you will get multiple disjoint lists. (All
	 * these lists will be circular.)
	 */
	for (i=0; i<SIZE; i++) {
		array[i].e = &array[(i+stride) % SIZE];
	}

	/*
	 * Traverse the list. We stop after hitting each element once.
	 *
	 * (If STRIDE was even, this will hit some elements more than 
	 * once and others not at all.)
	 */
	e = &array[0];
	for (i=0; i<SIZE; i++) {
		if (i % stride == 0) {
			putchar('.');
		}
		e = e->e;
	}

	printf("\nDone!\n");
	return 0;
}
