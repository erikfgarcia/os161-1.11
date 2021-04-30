
#include <types.h>
#include <kern/unistd.h>
#include <kern/errno.h>
#include <lib.h>
#include <addrspace.h>
#include <thread.h>
#include <curthread.h>
#include <vm.h>
#include <vfs.h>
#include <test.h>
#include <pid.h>
#include <machine/spl.h>

/*


This system call works exactly like getpid(), except that it returns the parent process’ pid. Note that,
the getppid() system call is not included in the OS/161 man pages, and you can assume the following
spec:
7getppid() will return the process ID of the parent of the calling process. This will be either the ID of
the process that created this process using fork() (if the parent hasn’t terminated yet), or -1 (if the
parent has already terminated).



*/


// return parent PID for current process
pid_t sys_getppid() {
	// get parent thread (process), return its PID
	//return curthread->parent->pid
	
	// check if parent has exited and exists
	// return PID if not exited, else -1
int spl = splhigh();

	if(curthread->parent == NULL) {
		// parent does not exist
		 splx(spl);
		return -1;
	}else {
		 splx(spl);
		// parent exists and has not exited
		return curthread->parent->pid;
	}
}




