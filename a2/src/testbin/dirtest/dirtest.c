/*
 * dirtest.c
 *
 * 	Tests your hierarchical directory implementation by creating
 * 	and deleting directories.
 *
 *      Works in the current directory.
 *
 *      Intended for the file system assignment. Should run (on SFS)
 *      when that assignment is complete.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <err.h>

#define MAXLEVELS       5

int
main(void)
{
	int i;
	const char *onename = "testdir";
	char dirname[512];

	strcpy(dirname, onename);

	for (i=0; i<MAXLEVELS; i++) {
		printf("Creating directory: %s\n", dirname);

		if (mkdir(dirname, 0755)) {
			err(1, "%s: mkdir", dirname);
		}
		
		strcat(dirname, "/");
		strcat(dirname, onename);
	}

	printf("Passed directory creation test.\n");

	for (i=0; i<MAXLEVELS; i++) {
		dirname[strlen(dirname) - strlen(onename) - 1] = 0;

		printf("Removing directory: %s\n", dirname);

		if (rmdir(dirname)) {
			err(1, "%s: rmdir", dirname);
		}
	}
	printf("Passed directory removal test.\n");

	return 0;
}

