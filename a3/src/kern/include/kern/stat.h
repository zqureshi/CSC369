#ifndef _KERN_STAT_H_
#define _KERN_STAT_H_

/*
 * Structure for stat (call to get file information)
 */

struct stat {
	u_int32_t st_mode;	/* protection mode and file type */
	u_int32_t st_nlink;	/* number of hard links */
	off_t st_size;		/* file size (bytes) */
	u_int32_t st_blocks;	/* number of blocks file is using */
};

/*
 * File types for st_mode.
 * The permissions are the low 12 bits; in OS/161 we don't use those.
 * (Unless you're implementing security and permissions and doing it
 * completely Unix-style.)
 */

#define S_IFMT  070000		/* mask for type of file */
#define S_IFREG 010000		/* ordinary regular file */
#define S_IFDIR 020000		/* directory */
#define S_IFLNK 030000		/* symbolic link */
#define S_IFCHR 040000		/* character device */
#define S_IFBLK 050000		/* block device */

/*
 * Macros for testing a mode value
 */
#define S_ISREG(mode)	(((mode) & S_IFMT) == S_IFREG)	/* regular file */
#define S_ISDIR(mode)	(((mode) & S_IFMT) == S_IFDIR)	/* directory */
#define S_ISLNK(mode)	(((mode) & S_IFMT) == S_IFLNK)	/* symlink */
#define S_ISCHR(mode)	(((mode) & S_IFMT) == S_IFCHR)	/* char device */
#define S_ISBLK(mode)	(((mode) & S_IFMT) == S_IFBLK)	/* block device */

#endif /* _KERN_STAT_H_ */
