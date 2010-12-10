/*
 * lseek
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

#include "config.h"
#include "test.h"

static
void
lseek_fd_device(void)
{
	int fd, rv;

	fd = open("null:", O_RDONLY);
	if (fd<0) {
		warn("UH-OH: opening null: failed");
		return;
	}

	rv = lseek(fd, 309, SEEK_SET);
	report_test(rv, errno, ESPIPE, "lseek on device");

	close(fd);
}

static
void
lseek_file_stdin(void)
{
	int fd, fd2, rv, status;
	const char slogan[] = "There ain't no such thing as a free lunch";
	size_t len = strlen(slogan);
	pid_t pid;

	/* fork so we don't affect our own stdin */
	pid = fork();
	if (pid<0) {
		warn("UH-OH: fork failed");
		return;
	}
	else if (pid!=0) {
		/* parent */
		rv = waitpid(pid, &status, 0);
		if (rv<0) {
			warn("UH-OH: waitpid failed");
		}
		if (status!=0) {
			warn("UH-OH: subprocess exited with code %d", status);
		}
		return;
	}

	/* child */

	fd = open_testfile(NULL);
	if (fd<0) {
		_exit(0);
	}

	/* 
	 * Move file to stdin.
	 * Use stdin (rather than stdout or stderr) to maximize the
	 * chances of detecting any special-case handling of fds 0-2.
	 * (Writing to stdin is fine as long as it's open for write,
	 * and it will be.)
	 */
	fd2 = dup2(fd, STDIN_FILENO);
	if (fd2<0) {
		warn("UH-OH: dup2 to stdin failed");
		close(fd);
		remove(TESTFILE);
		_exit(0);
	}
	if (fd2 != STDIN_FILENO) {
		warn("UH-OH: dup2 returned wrong file handle");
		close(fd);
		remove(TESTFILE);
		_exit(0);
	}
	close(fd);

	rv = write(STDIN_FILENO, slogan, len);
	if (rv<0) {
		warn("UH-OH: write to %s (via stdin) failed", TESTFILE);
		remove(TESTFILE);
		_exit(0);
	}

	if ((unsigned)rv != len) {
		warnx("UH-OH: write to %s (via stdin) got short count",
		      TESTFILE);
		remove(TESTFILE);
		_exit(0);
	}

	rv = lseek(STDIN_FILENO, 0, SEEK_SET);
	report_test(rv, errno, 0, "lseek stdin when open on file (try 1)");

	rv = lseek(STDIN_FILENO, 0, SEEK_END);
	report_test(rv, errno, 0, "lseek stdin when open on file (try 2)");

	remove(TESTFILE);
	_exit(0);
}

static
void
lseek_loc_negative(void)
{
	int fd, rv;

	fd = open_testfile(NULL);
	if (fd<0) {
		return;
	}

	rv = lseek(fd, -309, SEEK_SET);
	report_test(rv, errno, EINVAL, "lseek to negative offset");

	close(fd);
	remove(TESTFILE);
}

static
void
lseek_whence_inval(void)
{
	int fd, rv;

	fd = open_testfile(0);
	if (fd<0) {
		return;
	}

	rv = lseek(fd, 0, 3594);
	report_test(rv, errno, EINVAL, "lseek with invalid whence code");

	close(fd);
	remove(TESTFILE);
}

static
void
lseek_loc_pasteof(void)
{
	const char *message = "blahblah";
	int fd;
	off_t pos;

	fd = open_testfile(message);
	if (fd<0) {
		return;
	}

	pos = lseek(fd, 5340, SEEK_SET);
	if (pos == -1) {
		warn("FAILURE: lseek past EOF failed");
		goto out;
	}
	if (pos != 5340) {
		warnx("FAILURE: lseek to 5340 got offset %ld", (long) pos);
		goto out;
	}

	pos = lseek(fd, -50, SEEK_CUR);
	if (pos == -1) {
		warn("FAILURE: small seek beyond EOF failed");
		goto out;
	}
	if (pos != 5290) {
		warnx("FAILURE: SEEK_CUR to 5290 got offset %ld", (long) pos);
		goto out;
	}

	pos = lseek(fd, 0, SEEK_END);
	if (pos == -1) {
		warn("FAILURE: seek to EOF failed");
		goto out;
	}

	if (pos != (off_t) strlen(message)) {
		warnx("FAILURE: seek to EOF got %ld (should be %d)", 
		      (long) pos, strlen(message));
		goto out;
	}

	warnx("passed: seek past/to EOF");

    out:
	close(fd);
	remove(TESTFILE);
	return;
}

void
test_lseek(void)
{
	test_lseek_fd();

	lseek_fd_device();
	lseek_file_stdin();
	lseek_loc_negative();
	lseek_loc_pasteof();
	lseek_whence_inval();
}
