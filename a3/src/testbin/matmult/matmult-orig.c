/* matmult-orig.c 
 *    Test program to do matrix multiplication on large arrays.
 *
 *    Intended to stress virtual memory system.
 *
 *    This is the original CS161 matmult program. Unfortunately,
 *    because matrix multiplication is order N^2 in space and N^3 in
 *    time, when this is made large enough to be an interesting VM
 *    test, it becomes so large that it takes hours to run.
 *
 *    So you probably want to just run matmult, which has been
 *    gimmicked up to be order N^3 in space and thus have a tolerable
 *    running time. This version is provided for reference only.
 *
 *    Once the VM assignment is complete your system should be able to
 *    survive this, if you have the patience to run it.
 */

#include <unistd.h>
#include <stdio.h>

#define Dim 	360	/* sum total of the arrays doesn't fit in 
			 * physical memory 
			 */

#define RIGHT  46397160		/* correct answer */

int A[Dim][Dim];
int B[Dim][Dim];
int C[Dim][Dim];

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
		 C[i][j] += A[i][k] * B[k][j];

    printf("matmult-orig finished.\n");
    r = C[Dim-1][Dim-1];
    printf("answer is: %d (should be %d)\n", r, RIGHT);
    if (r != RIGHT) {
	    printf("FAILED\n");
    }
    else {
	    printf("Passed.\n");
    }
    return 0;
}
