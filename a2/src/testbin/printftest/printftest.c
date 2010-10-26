#include <unistd.h>
#include <stdio.h>
#include<string.h>

/*
 * printftest - Test implementation of the printf library function
 * using the printchar syscall
 * Usage: printftest
 *
 * Just calls printf() with some strings, some with format chars.
 *
 */

int
main()
{
	int result;
	int len;

	const char *teststring = "This is another simple string.\n";
	len = strlen(teststring);
	
	result = printf("This is a simple string.\n");
	result = printf("%s",teststring);

	if (result != len) {
	  printchar('E');
	  printchar('R');
	  printchar('R');
	  printchar('O');
	  printchar('R');
	  printchar('\n');
	} else {
	  printf("Everything ok, printing teststring with len %d returned %d chars printed\n",len,result);
	}

	return 0;
}
