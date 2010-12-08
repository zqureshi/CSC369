#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <limits.h>

/*
 * pwd - print working directory.
 * Usage: pwd
 *
 * Just uses the getcwd library call (which in turn uses the __getcwd
 * system call.)
 * 
 * Takes usual argc/argv arguments, but does not use them.
 */

int
cmd_pwd(int argc, char *argv[])
{
        (void)argc;
	(void)argv;

	char buf[PATH_MAX+1], *p;

	p = getcwd(buf, sizeof(buf));
	if (p == NULL) {
		warn("pwd - .");
		return 0;
	}
	printf("%s\n", buf);
	return 0;
}
