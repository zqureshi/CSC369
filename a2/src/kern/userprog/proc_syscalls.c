#include <types.h>
#include <pid.h>
#include <thread.h>
#include <curthread.h>
#include <lib.h>

/* Return curthread's pid */
pid_t sys_getpid()
{
  pid_t curr_pid  =  curthread->t_pid;
  DEBUG(DB_SYSCALL, "curthread->t_pid: %d\n", curr_pid);
  return curr_pid;
}

