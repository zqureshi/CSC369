/*
 * File handles and file tables.
 * New for ASST3
 */

#include <types.h>
#include <kern/errno.h>
#include <kern/limits.h>
#include <kern/stat.h>
#include <kern/unistd.h>
#include <lib.h>
#include <synch.h>
#include <uio.h>
#include <thread.h>
#include <curthread.h>
#include <vfs.h>
#include <vnode.h>
#include <file.h>
#include <syscall.h>

/* ASST3: custom functions*/

/* Get a empty file descriptor on each call, else return FOPEN_MAX */
int filetable_getfd(){
  struct filetable *ft = curthread->t_filetable;

  int fd;
  for(fd=0; fd<FOPEN_MAX; fd++){
    if(ft->ft_openfiles[fd] == NULL)
      return fd;
  }

  return fd;
}

/*** openfile functions ***/

/*
 * file_open
 * opens a file, places it in the filetable, sets RETFD to the file
 * descriptor. the pointer arguments must be kernel pointers.
 * NOTE -- the passed in filename must be a mutable string.
 * 
 * ASST3: As per the OS/161 man page for open(), you do not need 
 * to do anything with the "mode" argument.
 */
int
file_open(char *filename, int flags, int mode, int *retfd)
{
  (void)mode;

  /* Perform error checking */
  int fd = filetable_getfd();
  if(fd == FOPEN_MAX){
    return EMFILE;
  }

  /* Allocate a new file structure */
  struct openfile *of = kmalloc(sizeof (struct openfile));
  if(of == NULL){
    return ENOMEM;
  }

  /* Initialize the file structure */
  of->of_accmode = flags;
  of->of_offset = 0;
  of->of_refcount = 1;

  int result = vfs_open(filename, flags, &(of->of_vnode));
  if(result){
    kfree(of->of_vnode);
    kfree(of);
    return result;
  }

  /* Everything OK, add file to filetable */
  curthread->t_filetable->ft_openfiles[fd] = of;

  *retfd = fd;
  return 0;
}


/* 
 * file_close
 * knock off the refcount, freeing the memory if it goes to 0.
 */
int
file_close(int fd)
{
  struct openfile *of;
  int result = filetable_findfile(fd, &of);
  if(result){
    return result;
  }

  of->of_refcount -= 1;

  /* If refcount hits zero, free up struct */
  if(of->of_refcount == 0){
    vfs_close(of->of_vnode);
    kfree(of);
    /* mark file descriptor as empty */
    curthread->t_filetable->ft_openfiles[fd] = NULL;
  }

  return 0;
}

/*** filetable functions ***/

/* 
 * filetable_init
 * pretty straightforward -- allocate the space, set up 
 * first 3 file descriptors for stdin, stdout and stderr,
 * and initialize all other entries to NULL.
 * 
 * Should set curthread->t_filetable to point to the
 * newly-initialized filetable.
 * 
 * Should return non-zero error code on failure.  Currently
 * does nothing but returns success so that loading a user
 * program will succeed even if you haven't written the
 * filetable initialization yet.
 */
int
filetable_init()
{
  /* Allocate File Table */
  struct filetable *ft = kmalloc(sizeof (struct filetable));
  if(ft == NULL){
    return ENOMEM;
  }

  /* Initialize everything to NULL */
  int i;
  for(i=0; i<FOPEN_MAX; i++){
    ft->ft_openfiles[i] = NULL;
  }

  /* point t_filetable to ft */
  curthread->t_filetable = ft;

  /* Setup standard console streams */
  int result, fd;
  char path[5];

  /* Setup STDIN fd 0 */
  strcpy(path, "con:");
  result = file_open(path, O_RDWR, 0, &fd);
  if(result){
    return result;
  }

  /* Setup STDOUT fd 1 */
  strcpy(path, "con:");
  result = file_open(path, O_RDWR, 0, &fd);
  if(result){
    return result;
  }

  /* Setup STDERR fd 2 */
  strcpy(path, "con:");
  result = file_open(path, O_RDWR, 0, &fd);
  if(result){
    return result;
  }

  return 0;
}


/*
 * filetable_destroy
 * closes the files in the file table, frees the table.
 */
void
filetable_destroy(struct filetable *ft)
{
  if(ft == NULL){
    return;
  }

  int i;
  for(i=0; i<FOPEN_MAX; i++){
    if(ft->ft_openfiles[i] != NULL)
      file_close(i);
  }

  kfree(ft);
}	

/*
 * filetable_findfile
 * verifies that the file descriptor "fd" is valid and actually 
 * references an open file, setting the "file" to point to the
 * struct openfile object at that index (if fd is valid).
 */
int
filetable_findfile(int fd, struct openfile **file)
{
  if(fd < 0 || fd >= FOPEN_MAX){
    return EBADF;
  }

  if(curthread->t_filetable->ft_openfiles[fd] == NULL){
    return EBADF;
  }

  *file = curthread->t_filetable->ft_openfiles[fd];
  return 0;
}
