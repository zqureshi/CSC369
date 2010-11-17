/* 
 * huge.c
 *
 * 	Tests the VM system by accessing a large array (sparse) that
 *	cannot fit into memory.
 *
 * When the VM system assignment is done, your system should be able
 * to run this successfully.
 */

#include <stdio.h>
#include <stdlib.h>

#define PageSize	4096
#define NumPages	512

int sparse[NumPages][PageSize];	/* use only the first element in the row */

int
main()
{
	int i,j;

	printf("Entering the huge program - I will stress test your VM\n");

	/* move number in so that sparse[i][0]=i */
	for (i=0; i<NumPages; i++) {
		sparse[i][0]=i;
	}
	
	printf("stage [1] done\n");
	
	/* increment each location 5 times */
	for (j=0; j<5; j++) {
		for (i=0; i<NumPages; i++) {
			sparse[i][0]++;
		}
		printf("stage [2.%d] done\n", j);
	}
	
	printf("stage [2] done\n");
	
	/* check if the numbers are sane */
	for (i=NumPages-1; i>=0; i--) {
		if (sparse[i][0]!=i+5) {
			printf("BAD NEWS!!! - your VM mechanism has a bug!\n");
			exit(1);
		}
	}
	
	printf("You passed!\n");
	
	return 0;
}

