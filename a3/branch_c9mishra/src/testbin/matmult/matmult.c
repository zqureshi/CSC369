/* matmult.c 
 *    Test program to do matrix multiplication on large arrays.
 *
 *    This version uses a storage-inefficient technique to get a
 *    shorter running time for the same memory usage.
 *
 *    Intended to stress virtual memory system.
 *
 *    Once the VM system assignment is complete your system should be
 *    able to survive this.
 */

#include <unistd.h>
#include <stdio.h>

#define Dim 	72	/* sum total of the arrays doesn't fit in 
			 * physical memory 
			 */

#define RIGHT  8772192		/* correct answer */

int A[Dim][Dim];
int B[Dim][Dim];
int C[Dim][Dim];
int T[Dim][Dim][Dim];

int
main()
{
    int i, j, k, r;

    for (i = 0; i < Dim; i++)		/* first initialize the matrices */
	for (j = 0; j < Dim; j++) {
	     A[i][j] = i;
	     B[i][j] = j;
	     C[i][j] = 0;
	}

    for (i = 0; i < Dim; i++)		/* then multiply them together */
	for (j = 0; j < Dim; j++)
            for (k = 0; k < Dim; k++)
		T[i][j][k] = A[i][k] * B[k][j];

    for (i = 0; i < Dim; i++)
	for (j = 0; j < Dim; j++)
            for (k = 0; k < Dim; k++)
		C[i][j] += T[i][j][k];

    r = 0;
    for (i = 0; i < Dim; i++)
	    r += C[i][i];

    printf("matmult finished.\n");
    printf("answer is: %d (should be %d)\n", r, RIGHT);
    if (r != RIGHT) {
	    printf("FAILED\n");
	    return 1;
    }
    printf("Passed.\n");
    return 0;
}
