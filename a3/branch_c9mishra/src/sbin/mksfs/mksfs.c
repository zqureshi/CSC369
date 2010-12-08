#include <sys/types.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <err.h>

#include "support.h"
#include "kern/sfs.h"


#ifdef HOST

#include <netinet/in.h> // for arpa/inet.h
#include <arpa/inet.h>  // for ntohl
#include "hostcompat.h"
#define SWAPL(x) ntohl(x)
#define SWAPS(x) ntohs(x)

#else

#define SWAPL(x) (x)
#define SWAPS(x) (x)

#endif

#include "disk.h"

#define MAXBITBLOCKS 32

/* Block number for the initial root directory contents */
static int rootdir_data_block;

static
void
check(void)
{
	assert(sizeof(struct sfs_super)==SFS_BLOCKSIZE);
	assert(sizeof(struct sfs_inode)==SFS_BLOCKSIZE);
	assert(SFS_BLOCKSIZE % sizeof(struct sfs_dir) == 0);
}

static
void
writesuper(const char *volname, u_int32_t nblocks)
{
	struct sfs_super sp;

	bzero((void *)&sp, sizeof(sp));

	if (strlen(volname) >= SFS_VOLNAME_SIZE) {
		errx(1, "Volume name %s too long", volname);
	}

	sp.sp_magic = SWAPL(SFS_MAGIC);
	sp.sp_nblocks = SWAPL(nblocks);
	strcpy(sp.sp_volname, volname);

	diskwrite(&sp, SFS_SB_LOCATION);
}

static
void
writerootdir(void)
{
	struct sfs_inode sfi;
	struct sfs_dir sfd[SFS_BLOCKSIZE / sizeof(struct sfs_dir)];

	assert(rootdir_data_block > 0);
	assert(sizeof(sfd) >= sizeof(struct sfs_dir) * 2);

	bzero((void *)&sfi, sizeof(sfi));
	bzero((void *)sfd, sizeof(sfd));

	sfi.sfi_size = SWAPL(sizeof(struct sfs_dir) * 2);
	sfi.sfi_type = SWAPS(SFS_TYPE_DIR);
	sfi.sfi_linkcount = SWAPS(2);
	sfi.sfi_direct[0] = SWAPL(rootdir_data_block);

	diskwrite(&sfi, SFS_ROOT_LOCATION);

	sfd[0].sfd_ino = SWAPL(SFS_ROOT_LOCATION);
	strcpy(sfd[0].sfd_name, ".");
	sfd[1].sfd_ino = SWAPL(SFS_ROOT_LOCATION);
	strcpy(sfd[1].sfd_name, "..");

	diskwrite(sfd, rootdir_data_block);
}

static char bitbuf[MAXBITBLOCKS*SFS_BLOCKSIZE];

static
void
doallocbit(u_int32_t bit)
{
	u_int32_t byte = bit/CHAR_BIT;
	unsigned char mask = (1<<(bit % CHAR_BIT));

	assert((bitbuf[byte] & mask) == 0);
	bitbuf[byte] |= mask;
}

static
void
writebitmap(u_int32_t fsblocks)
{

	u_int32_t nbits = SFS_BITMAPSIZE(fsblocks);
	u_int32_t nblocks = SFS_BITBLOCKS(fsblocks);
	char *ptr;
	u_int32_t i;

	if (nblocks > MAXBITBLOCKS) {
		errx(1, "Filesystem too large "
		     "- increase MAXBITBLOCKS and recompile");
	}

	doallocbit(SFS_SB_LOCATION);
	doallocbit(SFS_ROOT_LOCATION);
	for (i=0; i<nblocks; i++) {
		doallocbit(SFS_MAP_LOCATION+i);
	}
	rootdir_data_block = SFS_MAP_LOCATION + nblocks;
	doallocbit(rootdir_data_block);
	for (i=fsblocks; i<nbits; i++) {
		doallocbit(i);
	}

	for (i=0; i<nblocks; i++) {
		ptr = bitbuf + i*SFS_BLOCKSIZE;
		diskwrite(ptr, SFS_MAP_LOCATION+i);
	}
}

int
main(int argc, char **argv)
{
	u_int32_t size, blocksize;
	char *volname, *s;

#ifdef HOST
	hostcompat_init(argc, argv);
#endif

	if (argc!=3) {
		errx(1, "Usage: mksfs device/diskfile volume-name");
	}

	check();

	volname = argv[2];

	/* Remove one trailing colon from volname, if present */
	s = strchr(volname, ':');
	if (s != NULL) {
		if (strlen(s)!=1) {
			errx(1, "Illegal volume name %s", volname);
		}
		*s = 0;
	}

	/* Don't allow slashes */
	s = strchr(volname, '/');
	if (s != NULL) {
		errx(1, "Illegal volume name %s", volname);
	}

	opendisk(argv[1]);
	blocksize = diskblocksize();

	if (blocksize!=SFS_BLOCKSIZE) {
		errx(1, "Device has wrong blocksize %u (should be %u)\n",
		     blocksize, SFS_BLOCKSIZE);
	}
	size = diskblocks();

	writesuper(volname, size);
	writebitmap(size);
	writerootdir();

	closedisk();

	return 0;
}
