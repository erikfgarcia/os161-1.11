Write any inforamtion that you think is important here.

we may use this document to do the design document

* kern/include/kern/callno.h This file has the system call numbers these are used in arch/mips/mips/syscall.c  when the system call handler fuction 
  "mips_syscall(struct trapframe *tf)" takes care of these calls.   
 
* /include/callno.h These are the user-level interfaces. The fuctions defined here can be called by any user-level program. But we have to implement them!    

* kern/include/kern/unistd.h This file had the constants for system call these are used when implementing write(), open(), read() reboot() "reboo() is the onlyone
  implemeted" lseek() I think we need the definitions for the last one only 

* kern/include/kern/limits.h this file defines NAME_MAX PATH_MAX OPEN_MAX we may not need these but if we want to define a "MAX" macro we can do it here.

* kern/include/thread.h  here we modify the thread 

* kern/include/syscall.h  this is the entry points for system call implementations. I took care of this   

* kern/include/queue.h and kern/include/array.h we may need to use these data structures 


* kern/include/addrspace.h 


* arch/mips/include/trapframe.h  Structure describing what is saved on the stack during entry to the exception handler.

* arch/mips/mips/syscall.c  this is the system call handler I took care of this already but it may need changes.
  we find mips_syscall() here this is the handler. passes the arguments extracted from the trapframe
  - make sure to increment the process counter or it will restar the same sytem call it->tf_epc+=4
  - if error store the error code in tf->tf_v0 and set tf->tf_a3 to 1
  - if not error store the return value in tf->tf_v0 and set tf->tf_a3 to 0   
  *these are implemete we to undestand this to implement our system calls

* arch/mips/mips/trap.c  we need to take care of kill_curthread().. done it I think. 


* trap-frame: the system state is saved in an structure called trapframe when there is an execption accurs. we need this to restore the system  


/*******************************************************************************************************************************/

* These are our implementations in addition to the 6 files
    ~/os161/os161-1.11/kern/include/pid.h
    ~/os161/os161-1.11/kern/include/children.h
    ~/os161/os161-1.11/kern/thread/pid.c


* kern/userprog/ has the 6 files  we are working on 
  - I already added these 6 files to kern/conf/conf.kern 
  - I declared these the fucntions in kern/include/syscall.h
 
  > sys_getpid: just return the pid of the execution process 
  
  > sys_getpid: return the pid of the parent of the execution process. if parent has exited or pid do not exits  return -1
    we need to  write a test for this 
  
  > sys_execv: replace executing program image with a nre process image. pid is not change
        /*parameters*/
	- program: path name of the program to run  
	- args:tf->_a0 and tf->_a1
        
        /*implemetation*/ 
        - most implemetation is already done in runprogram.c !!!!!! :)  see funtion runprogram() it replaces the image but do not pass arguments to the 
	  new program we to modify this fuanction and ... 
        	- we need to add: - copyin the arguments from user space to kernel space    
		 		  - create a new adresss space: as_create()
                                  - alloate a stack on it: as_defined_stack()
	  			  - activate it: as_activate()
				  - copyout the arguments back onto the new stack
				  - return to usermode by calling md_usermode()		 
                     /*the most dificult is the copyin and copyout*/
         - error to return : see the man page description that I put at the beging of execv.c 
 
  >  sys_fork
	- duplicates the current process. the child will have a unique id
	- child process return 0
        - parent get the pid of the child 
        - if error return -1 do not create a process
        - most of the work is done in thread.c (thread_fork()) !!!! :) we need to add
		- create pid when creating a new process and added to the process table 
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
  
  > sys_waitpid 
        - waits for the process with the pid to exi
	- returns its exit code with integer pointer status 
  	- ignore options pid should be assigned to status 
	- add restriction on witch process can have interest in the pid 
	- tf->tf_a0, (userptr_t) tf->tf_a1, tf_>tfa2, &retval
	- interest: add restrictions or mechanism to avoid deadlock 
        - return the pid and assign to exit status
        - on error return -1 and set the proper erro code
        - code : see descrpitopn

 > sys_exit
	- causes the current process to teminate
	- pid of the exiting process cannot be reused if another process is intested in it 
        - code is the exitcode that will be assigned  
  
/******************************/

* This is how I modified the thread structure
 	Each tread is a process i added this 
	pid 
	parent 
	children 
	exit_status

       in detroy() 
       I make sure the record is updated when the thread exits  
       - there is a helper function is_only_one_thread() returns 1 is there is only one thread
 

* This is the pid implemetation 
  
   -pid_t new_pid() this give you a new pid;
	
these change the pid exit_status 
   -void pid_parent_exit(pid_t pid)  indicates that the process's parent has exited, if that is the case the pid doesn't need to be saved when the process exits
   -pid_process_exit(pid_t pid) indicate that the process  has exited, it can be recycled
   -void pid_free(pid_t pid) recycles the pid. use it when a thread with no parent exits 
   -int pid_is_used(pid_t pid) check if the pid is being used

*This is the Global Process table implementation 
info of all process


					   /****  Design Document:  ******/


* high-level description of how we approach the problem :

* detailed description of the implementation:
	structures...


* pro and cons of our approach:


* alternative we discarded: 


* The output of the tests:  
