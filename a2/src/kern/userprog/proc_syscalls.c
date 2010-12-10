#include <types.h>
#include <pid.h>
#include <addrspace.h>
#include <thread.h>
#include <curthread.h>
#include <lib.h>
#include <test.h>

/* Return curthread's pid */
pid_t sys_getpid()
{
  pid_t curr_pid  =  curthread->t_pid;
  DEBUG(DB_SYSCALL, "curthread->t_pid: %d\n", curr_pid);
  return curr_pid;
}

int sys_execv(userptr_t program, userptr_t args){
  /* copy progname into kernel space */
  char progname[256];
  strcpy(progname, (char *)program);

  /* Destroy Current address space */
  as_destroy(curthread->t_vmspace);
  curthread->t_vmspace = NULL;

  /* Do something with args for the time being*/
  (void)args;

  /* Load the new program */
  return runprogram(progname, (char **)args);
}
