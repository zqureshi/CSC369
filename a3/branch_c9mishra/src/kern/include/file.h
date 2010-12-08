/*
 * Declarations for file handle and file table management.
 * New for ASST3.
 */

#ifndef _FILE_H_
#define _FILE_H_

#include <kern/limits.h>

struct vnode;

/*** openfile section ***/

/* 
 * openfile struct 
 * note that there's not too much to keep track of, since the vnode does most
 * of that.  
 * In general, it would require synchronization, because openfile objects
 * could be shared between a parent process and the children that it fork()'s.
 * We are not supporting fork(), so the only sharing of openfile objects
 * will be within the same process (due to dup2()), and so we can 
 * get away without synchronization.
 */
struct openfile {
	struct vnode *of_vnode;
	off_t of_offset;
	int of_accmode;	/* from open: O_RDONLY, O_WRONLY, or O_RDWR */
	int of_refcount;
        // ASST3: You can add additional fields here if you wish.
};

/* opens a file (must be kernel pointers in the args) */
int file_open(char *filename, int flags, int mode, int *retfd);

/* closes a file */
int file_close(int fd);


/*** file table section ***/

/*
 * filetable struct
 * just an array of open files.  nice and simple.  doesn't require
 * synchronization, because a table can only be owned by a single process (on
 * inheritance in fork, the table would be copied, but we are 
 * not supporting fork).
 */
struct filetable {
	struct openfile *ft_openfiles[FOPEN_MAX];
};

/* these all have an implicit arg of the curthread's filetable */
int filetable_init();
int filetable_findfile(int fd, struct openfile **file);
void filetable_destroy(struct filetable *ft);


/* ASST3: You may wish to add additional functions that operate on
 * the filetable to help implement some of the filetable-related
 * system calls.
 */

#endif /* _FILE_H_ */
