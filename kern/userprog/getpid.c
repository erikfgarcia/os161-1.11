

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



/*
getpid

OS/161 Reference Manual

Name

getpid - get process id

Library

Standard C Library (libc, -lc)

Synopsis

#include <unistd.h>
pid_t
getpid(void);

Description

getpid returns the process id of the current process.

Errors

getpid does not fail
*/


// return pid of current process
pid_t sys_getpid() {
	// return pid of current thread (process)
	return curthread->pid;
}


