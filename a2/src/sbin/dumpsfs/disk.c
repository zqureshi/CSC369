#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <err.h>

#include "support.h"
#include "disk.h"

#define HOSTSTRING "System/161 Disk Image"
#define BLOCKSIZE  512

#ifndef EINTR
#define EINTR 0
#endif

static int fd=-1;
static u_int32_t nblocks;

void
opendisk(const char *path)
{
	struct stat statbuf;

	assert(fd<0);
	fd = open(path, O_RDWR);
	if (fd<0) {
		err(1, "%s", path);
	}
	if (fstat(fd, &statbuf)) {
		err(1, "%s: fstat", path);
	}

	nblocks = statbuf.st_size / BLOCKSIZE;

#ifdef HOST
	nblocks--;

	{
		char buf[64];
		int len;

		do {
			len = read(fd, buf, sizeof(buf)-1);
			if (len < 0 && (errno==EINTR || errno==EAGAIN)) {
				continue;
			}
		} while (0);

		buf[len] = 0;
		buf[strlen(HOSTSTRING)] = 0;

		if (strcmp(buf, HOSTSTRING)) {
			errx(1, "%s: Not a System/161 disk image", path);
		}
	}
#endif
}

u_int32_t
diskblocksize(void)
{
	assert(fd>=0);
	return BLOCKSIZE;
}

u_int32_t
diskblocks(void)
{
	assert(fd>=0);
	return nblocks;
}

void
diskwrite(const void *data, u_int32_t block)
{
	const char *cdata = data;
	u_int32_t tot=0;
	int len;

	assert(fd>=0);

#ifdef HOST
	// skip over disk file header
	block++;
#endif

	if (lseek(fd, block*BLOCKSIZE, SEEK_SET)<0) {
		err(1, "lseek");
	}

	while (tot < BLOCKSIZE) {
		len = write(fd, cdata + tot, BLOCKSIZE - tot);
		if (len < 0) {
			if (errno==EINTR || errno==EAGAIN) {
				continue;
			}
			err(1, "write");
		}
		if (len==0) {
			err(1, "write returned 0?");
		}
		tot += len;
	}
}

void
diskread(void *data, u_int32_t block)
{
	char *cdata = data;
	u_int32_t tot=0;
	int len;

	assert(fd>=0);

#ifdef HOST
	// skip over disk file header
	block++;
#endif

	if (lseek(fd, block*BLOCKSIZE, SEEK_SET)<0) {
		err(1, "lseek");
	}

	while (tot < BLOCKSIZE) {
		len = read(fd, cdata + tot, BLOCKSIZE - tot);
		if (len < 0) {
			if (errno==EINTR || errno==EAGAIN) {
				continue;
			}
			err(1, "read");
		}
		if (len==0) {
			err(1, "unexpected EOF in mid-sector");
		}
		tot += len;
	}
}

void
closedisk(void)
{
	assert(fd>=0);
	if (close(fd)) {
		err(1, "close");
	}
	fd = -1;
}
