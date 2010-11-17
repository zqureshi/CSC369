#ifndef _SYS_STAT_H_
#define _SYS_STAT_H_

/*
 * Get struct stat and all the #defines from the kernel
 */
#include <kern/stat.h>

/*
 * stat is the same as fstat, only on a file that isn't already
 * open. lstat is the same as stat, only if the name passed names a
 * symlink, information about the symlink is returned rather than
 * information about the file it points to. You don't need to
 * implement lstat unless you're implementing symbolic links.
 */
int fstat(int filehandle, struct stat *buf);
int stat(const char *path, struct stat *buf);
int lstat(const char *path, struct stat *buf);

/* 
 * The second argument to mkdir is the mode for the new directory.
 * Unless you're implementing security and permissions, you can
 * (and should) ignore it. See notes in unistd.h.
 */
int mkdir(const char *dirname, int ignore);


#endif /* _SYS_STAT_H_ */
