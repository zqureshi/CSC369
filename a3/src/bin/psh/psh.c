
/*
 * psh - pseudo-shell
 *
 * Executes certain file system operations by calling
 * appropriate functions.  Does not fork/exec programs
 * to execute commands.
 *
 * Usage:
 *     psh
 *     
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <err.h>

#ifdef HOST
#include "hostcompat.h"
#endif

/* no NARG_MAX on most unixes */
#define NARG_MAX 1024
#define ARG_MAX 32768

/*
 * chdir
 * just an interface to the system call.  no concept of home directory, so
 * require the directory.
 */
static
int
cmd_chdir(int ac, char *av[])
{
	if (ac == 2) {
		if (chdir(av[1])) {
			warn("chdir");
			return 1;
		}
		return 0;
	}
	printf("Usage: chdir dir\n");
	return 1;
}

/*
 * exit
 * pretty simple.  allow the user to choose the exit code if they want,
 * otherwise default to 0 (success).
 */
static
int
cmd_exit(int ac, char *av[])
{
	int code;

	if (ac == 1) {
		code = 0;
	}
	else if (ac == 2) {
		code = atoi(av[1]);
	}
	else {
		printf("Usage: exit [code]\n");
		return 1;
	}
	
	exit(code);
	
	return 0; /* quell the compiler warning */
}


extern int cmd_cat(int, char **);
extern int cmd_cp(int, char **);
extern int cmd_ls(int, char **);
extern int cmd_mkdir(int, char **);
extern int cmd_rmdir(int, char **);
extern int cmd_rm(int, char **);
extern int cmd_pwd(int, char **);
extern int cmd_sync(int, char **);
extern int cmd_ln(int, char **);
extern int cmd_mv(int, char **);
extern int cmd_opentest(int, char **);

/*
 * a struct of the builtins associates the builtin name with the function that
 * executes it.  they must all take an argc and argv.
 */
static struct {
	const char *name;
	int (*func)(int, char **);
} builtins[] = {
	{ "cd",    cmd_chdir },
	{ "chdir", cmd_chdir },
	{ "exit",  cmd_exit },
	{ "cat",   cmd_cat },
	{ "cp",    cmd_cp },
	{ "ls",    cmd_ls },
	{ "mkdir", cmd_mkdir },
	{ "rmdir", cmd_rmdir },
	{ "rm",    cmd_rm },
	{ "pwd",   cmd_pwd },
	{ "sync",  cmd_sync },
	{ "ln",    cmd_ln },
        { "mv",    cmd_mv },
	{ "opentest", cmd_opentest },
	{ NULL, NULL }
};

/*
 * docommand
 * tokenizes the command line using strtok.  if there aren't any commands,
 * simply returns.  checks to see if it's a builtin, running it if it is.
 * Otherwise, this pseudo-shell can't handle it.
 */


static
int
docommand(char *buf)
{
	static char *args[NARG_MAX + 1];
	int nargs, i;
	char *s;

	nargs = 0;
	for (s = strtok(buf, " \t\r\n"); s; s = strtok(NULL, " \t\r\n")) {
		if (nargs >= NARG_MAX) {
			printf("%s: Too many arguments "
			       "(exceeds system limit)\n",
			       args[0]);
			return 1;
		}
		args[nargs++] = s;
	}
	args[nargs] = NULL;

	if (nargs==0) {
		/* empty line */
		return 0;
	}

	for (i=0; builtins[i].name; i++) {
		if (!strcmp(builtins[i].name, args[0])) {
			return builtins[i].func(nargs, args);
		}
	}

	/* Not a builtin; pseudo-shell can't handle it. */
	printf("%s: Not a built-in command.  Can't handle this.\n",
	       args[0]);

	return 0;
}

/*
 * getcmd
 * pulls valid characters off the console, filling the buffer.  
 * backspace deletes a character, simply by moving the position back.
 * a newline or carriage return breaks the loop, which terminates
 * the string and returns.
 *
 * if there's an invalid character or a backspace when there's nothing 
 * in the buffer, putchars an alert (bell).
 */
static
void
getcmd(char *buf, size_t len)
{
	size_t pos = 0;
	int done=0, ch;

	/*
	 * In the absence of a <ctype.h>, assume input is 7-bit ASCII.
	 */

	while (!done) {
		ch = getchar();
		if ((ch == '\b' || ch == 127) && pos > 0) {
			putchar('\b');
			putchar(' ');
			putchar('\b');
			pos--;
		}
		else if (ch == '\r' || ch == '\n') {
			putchar('\r');
			putchar('\n');
			done = 1;
		}
		else if (ch >= 32 && ch < 127 && pos < len-1) {
			buf[pos++] = ch;
			putchar(ch);
		}
		else {
			/* alert (bell) character */
			putchar('\a');
		}
	}
	buf[pos] = 0;
}	

/*
 * interactive
 * runs the interactive shell.  basically, just infinitely loops, grabbing
 * commands and running them (and printing the exit status if it's not
 * success.)
 */
static
void
interactive(void)
{
	static char buf[ARG_MAX];
	int status;

	while (1) {
		printf("OS/161$ ");
		getcmd(buf, sizeof(buf));
		status = docommand(buf);
		if (status) {
			printf("Command returned %d\n", status);
		}
	}
}


/* 
 * main
 * always run interactively
 */
int
main(int argc, char *argv[])
{
        (void)argc;
	(void)argv;

        interactive();
	return 0;
}

