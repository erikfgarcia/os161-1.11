/*
fork

OS/161 Reference Manual

Name

fork - copy the current process

Library

Standard C Library (libc, -lc)

Synopsis

#include <unistd.h>
pid_t
fork(void);

Description

fork duplicates the currently running process. The two copies are
identical, except that one (the "new" one, or "child"), has a new,
unique process id, and in the other (the "parent") the process id is
unchanged.

The process id must be greater than 0.

The two processes do not share memory or open file tables; this state is
copied into the new process, and subsequent modification in one process
does not affect the other.

However, the file handle objects the file tables point to are shared,
so, for instance, calls to lseek in one process can affect the other.

Return Values

On success, fork returns twice, once in the parent process and once in
the child process. In the child process, 0 is returned. In the parent
process, the process id of the new child process is returned.

On error, no new process is created, fork only returns once, returning
-1, and errno[1] is set according to the error encountered.

Errors

The following error codes should be returned under the conditions given.
Other error codes may be returned for other errors not mentioned here.

�� EAGAIN Too many processes already exist. ENOMEM Sufficient virtual
memory for the new process was not available.

*/


#include <kern/unistd.h>
#include <types.h>
#include <thread.h>
#include <curthread.h>
#include <lib.h>
#include <kern/errno.h>
#include <children.h>
#include <machine/pcb.h>
#include <machine/spl.h>
#include <machine/trapframe.h>
#include <addrspace.h>
//#include <filetable.h>

/*
sys_fork
	- duplicates the current process. the child will have a unique id
	- child process return 0
        - parent get the pid of the child 
        - if error return -1 do not create a process
        - most of the work is done in thread.c (thread_fork()) !!!! :) we need to add
		- create pid when creatinn a new process and added to the process table 
		- copy the trapframe of the parent  
		- copy the address space of the parent 
		- call thread_fork(), 
		- the child will start executing from this fuction "md_forkenry()" (if we chose to call it as an argument to thread_fork())
                - the child will have te parent's trapframe and address space as reference. these are passed as arguments 
		- create a new child trapframe (must be on the stack, "kmalloc" wouldn't work)
 		- copy parent-trapframe's content to child trapframe and set tf_a3 to 0
		- set the current process ID to the newly generated child ID (since we are executing md_forkentry's, this is child) if not already set in
		   thread_fork()
		- set the trapframe's tf_v0 to 0 (child should return 0)
		- increment tf_epc by 4 (otherwise fork() gets invoke again) 
		- copy the passed address space to te current process adress space and activate it
		- give caontrol back to the usermode : call mipd_usermode() and pass the newlly created child trapframe 
	error to returns EAGAIN (too many processes already exits ) and ENOMEM (not enough virtual memory )



*/

pid_t sys_fork(struct trapframe *tf){
   
    
//copy the name of the parent    
    
    int spl = splhigh();

    char *child_name = kmalloc(sizeof(char) * (strlen(curthread->t_name)+9));
    if (child_name == NULL) {
        splx(spl);
        return ENOMEM;
    }

    child_name = strcpy(child_name, curthread->t_name);
    struct children *new_child = kmalloc(sizeof(struct children)); //
    if (new_child == NULL) {
        splx(spl);
        return ENOMEM;
    }
 
// we do not need to create a pid here because is created when the new thread is created in thread.c
//copy the trapframe of the parent  
//copy the address space of the parent


/*
 * * Create a new thread based on an existing one.
 *  * The new thread has name NAME, and starts executing in function FUNC.
 *   * DATA1 and DATA2 are passed to FUNC.
 * */
 
/*
int  thread_fork(const char *name,  void *data1, unsigned long data2,    void (*func)(void *, unsigned long),  struct thread **ret)
 */

//void  md_forkentry(struct trapframe *tf) this has to be mofied so 
   

     struct thread *child = NULL;

     void (*func_pt)(void *, unsigned long) = &md_forkentry;// this will create a warning 
  
    int result = thread_fork(strcat(child_name, "'s child"), tf, curthread->t_vmspace, func_pt, &child);
   
    if (result != 0) {
        kfree(new_child);
        splx(spl);
        return result;
    }  

    
    child->parent = curthread;
    //add new process to list of children
    new_child->pid = child->pid;
    new_child->finished = 0;
    new_child->exit_code = -2;
    new_child->next = curthread->children;
    curthread->children = new_child;
    
    
    int err = as_copy(curthread->t_vmspace, &child->t_vmspace); //copy the data to the child process's new address space
    if (err != 0) {
        
        child->t_vmspace = NULL;
        child->parent = NULL; //t
        md_initpcb(&child->t_pcb, child->t_stack, 0, 0, &thread_exit); 
        splx(spl);
        return err;
    }
   
    
    int retval = child->pid;
    splx(spl);
//    kprintf("I am the father: my child pid is %d\n", retval);
    return retval; 
}
