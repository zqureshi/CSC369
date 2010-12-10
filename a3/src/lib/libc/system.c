#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
 * system(): ANSI C
 *
 * Run a command.
 */

#define MAXCMDSIZE 2048
#define MAXARGS    128

int
system(const char *cmd)
{
	/*
	 * Ordinarily, you call the shell to process the command.
	 * But we don't know that the shell can do that. So, do it
	 * ourselves.
	 */

	char tmp[MAXCMDSIZE];
	char *argv[MAXARGS+1];
	int nargs=0;
	char *s;
	int pid, status;

	if (strlen(cmd) >= sizeof(tmp)) {
		errno = E2BIG;
		return -1;
	}
	strcpy(tmp, cmd);

	for (s = strtok(tmp, " \t"); s; s = strtok(NULL, " \t")) {
		if (nargs < MAXARGS) {
			argv[nargs++] = s;
		}
		else {
			errno = E2BIG;
			return 1;
		}
	}

	argv[nargs] = NULL;

	pid = fork();
	switch (pid) {
	    case -1:
		return -1;
	    case 0:
		/* child */
		execv(argv[0], argv);
		/* exec only returns if it fails */
		_exit(255);
	    default:
		/* parent */
		waitpid(pid, &status, 0);
		return status;
	}
}
