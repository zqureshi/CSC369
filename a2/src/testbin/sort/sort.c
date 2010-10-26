/* sort.c 
 *    Test program to sort a large number of integers.
 *
 *    Intention is to stress virtual memory system.
 *
 *    Once the virtual memory assignment is complete, your system
 *    should survive this.
 */

#include <stdlib.h>
#include <string.h>
#include <err.h>

/* Larger than physical memory */
#define SIZE  (144*1024)


/*
 * Quicksort.
 *
 * This used to be a bubble sort, which was ok but slow in nachos with
 * 4k of memory and SIZE of 1024. However, with SIZE of 147,456 bubble
 * sort is completely unacceptable.
 *
 * Also, quicksort has somewhat more interesting memory usage patterns.
 */

static
void
sort(int *arr, int size)
{
	static int tmp[SIZE];
	int pivot, i, j, k;

	if (size<2) {
		return;
	}

	pivot = size/2;
	sort(arr, pivot);
	sort(&arr[pivot], size-pivot);

	i = 0;
	j = pivot;
	k = 0;
	while (i<pivot && j<size) {
		if (arr[i] < arr[j]) {
			tmp[k++] = arr[i++];
		}
		else {
			tmp[k++] = arr[j++];
		}
	}
	while (i<pivot) {
		tmp[k++] = arr[i++];
	}
	while (j<size) {
		tmp[k++] = arr[j++];
	}

	memcpy(arr, tmp, size*sizeof(int));
}

////////////////////////////////////////////////////////////

static int A[SIZE];

static
void
initarray(void)
{
	int i;

	/*
	 * Initialize the array, with pseudo-random but deterministic contents.
	 */
	srandom(533);

	for (i = 0; i < SIZE; i++) {		
		A[i] = random();
	}
}

static
void
check(void)
{
	int i;

	for (i=0; i<SIZE-1; i++) {
		if (A[i] > A[i+1]) {
			errx(1, "Failed: A[%d] is %d, A[%d] is %d", 
			     i, A[i], i+1, A[i+1]);
		}
	}
	warnx("Passed.");
}

int
main(void)
{
	initarray();
	sort(A, SIZE);
	check();
	return 0;
}
