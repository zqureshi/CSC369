/*
 * hog.c
 * 	Spawned by several other user programs to test time-slicing.
 *
 * This does not differ from guzzle in any important way.
 */

int
main(void)
{
	volatile int i;

	for (i=0; i<50000; i++)
		;

	return 0;
}
