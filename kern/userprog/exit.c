/*void
_exit(int exitcode);
Description
Cause the current process to exit. The exit code exitcode is reported back to other process(es) via the waitpid() call. The process id of the exiting process should not be reused until all processes interested in collecting the exit code with waitpid have done so. (What "interested" means is intentionally left vague; you should design this.)
Return Values
_exit does not return. 
*/


#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <thread.h>
#include <curthread.h>
#include <lib.h>


/*
#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <machine/pcb.h>
#include <machine/spl.h>
#include <machine/trapframe.h>
#include <kern/callno.h>
#include <kern/unistd.h>
#include <syscall.h>
#include <thread.h>
#include <process.h>
#include <synch.h>
#include <uio.h>
#include <vfs.h>
#include <vnode.h>
#include <curthread.h>
#include <kern/limits.h>
#include <machine/vm.h>
#include <vm.h>
#include <kern/stat.h>
#include <fs.h>

#include "addrspace.h"

*/
/*
_exit

OS/161 Reference Manual

Name

_exit - terminate process

Library

Standard C Library (libc, -lc)

Synopsis

#include <unistd.h>
void
_exit(int exitcode);

Description

Cause the current process to exit. The exit code exitcode is reported
back to other process(es) via the waitpid()[1] call. The process id of
the exiting process should not be reused until all processes interested
in collecting the exit code with waitpid have done so. (What
"interested" means is intentionally left vague; you should design this.)

Return Values

_exit does not return.

*/


/*
void sys_exit(int exitcode) { 
	
   
    //report exit status	
    pid_t pid = curthread->pid; 
    struct process *parent = NULL;
    struct process *p = get_process(pid);
    if(p != NULL) {        
        //acquire the exit lock to see whether the child has exited or not
        lock_acquire(p->exitlock);                        
        p->exited = 1;                                
        
        p->exitcode = exitcode;        
        //sleep on the exitcode variable to be updated by the exiting child
        cv_broadcast(p->exitcv, p->exitlock);                       
        lock_release(p->exitlock);
    }
    
    //kprintf("\n Thread exitted %d\n", curthread->pid);
    thread_exit();

    panic("I shouldn't be here in sys__exit!");

  }
  
  */
  
  void sys__exit(int exitcode){
    /*
    The code for freeing resources used by a process are found in thread.c
    since we need to be able to free these resources on processes that exit
    abnormally (crash without calling _exit())
    */
    DEBUG(DB_THREADS, "Thread `%s` calling _exit()\n", curthread->t_name);
	curthread->exit_status = exitcode;
	thread_exit();
}

