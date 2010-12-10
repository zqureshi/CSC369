/*
 * faulter.c
 *
 * 	Tries to access an illegal address.
 *
 * When the system calls assignment is complete, this should run and
 * get killed without causing the kernel to panic.
 */

#include <stdio.h>

#define REALLY_BIG_ADDRESS	0x40000000

int
main(void) 
{
	volatile int i;

	printf("\nEntering the faulter program - I should die immediately\n");
	i = *(int *)REALLY_BIG_ADDRESS;    
	printf("I didn't get killed!  Program has a bug\n");
	return 0;
}
