

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

execv

OS/161 Reference Manual

Name

execv - execute a program

Library

Standard C Library (libc, -lc)

Synopsis

#include <unistd.h>
int
execv(const char *program, char **args);

Description

execv replaces the currently executing program with a newly loaded
program image. This occurs within one process; the process id is
unchanged.

The pathname of the program to run is passed as program. The args
argument is an array of 0-terminated strings. The array itself should be
terminated by a NULL pointer.

The argument strings should be copied into the new process as the new
process's argv[] array. In the new process, argv[argc] must be NULL.

By convention, argv[0] in new processes contains the name that was used
to invoke the program. This is not necessarily the same as program, and
furthermore is only a convention and should not be enforced by the
kernel.

The process file table and current working directory are not modified by
execve.

Return Values

On success, execv does not return; instead, the new program begins
executing. On failure, execv returns -1, and sets errno[1] to a suitable
error code for the error condition encountered.

Errors

The following error codes should be returned under the conditions given.
Other error codes may be returned for other errors not mentioned here.

�� ENODEV The device prefix of program did not exist. ENOTDIR A
non-final component of program was not a directory. ENOENT program did
not exist. EISDIR program is a directory. ENOEXEC program is not in a
recognizable executable file format, was for the wrong platform, or
contained invalid fields. ENOMEM Insufficient virtual memory is
available. E2BIG The total size of the argument strings is too large.
EIO A hard I/O error occurred. EFAULT One of the args is an invalid
pointer.


*/

int sys_execv(const char *program, char **args){
 

 	(void)program;
	(void)args;
 

	struct vnode *v;
	vaddr_t entrypoint, stackptr;
	int result;

	
	// copy name to kernel space
	char progname[128];
	size_t progname_len;
	result = copyinstr((userptr_t)program, progname, 128, &progname_len);
	if(result) {
		return result;
	} 

	// find number of args
	int offset = 0;
	unsigned long nargs = 0;
	while(*(args+offset) != NULL) {
		result = copyin((userptr_t)(args+offset), NULL, sizeof(int));
		if(result) return result;

		offset += 4;		// aligned by 4
		nargs++;
	}


	/* Open the file. */
	result = vfs_open(progname, O_RDONLY, &v);
	if (result) {
		return result;
	}

	/* We should be a new thread. */
	assert(curthread->t_vmspace == NULL);

	/* Create a new address space. */
	curthread->t_vmspace = as_create();
	if (curthread->t_vmspace==NULL) {
		vfs_close(v);
		return ENOMEM;
	}

	/* Activate it. */
	as_activate(curthread->t_vmspace);

	/* Load the executable. */
	result = load_elf(v, &entrypoint);
	if (result) {
		/* thread_exit destroys curthread->t_vmspace */
		vfs_close(v);
		return result;
	}

	/* Done with the file now. */
	vfs_close(v);

	/* Define the user stack in the address space */
	result = as_define_stack(curthread->t_vmspace, &stackptr);
	if (result) {
		/* thread_exit destroys curthread->t_vmspace */
		return result;
	} 


	int stack_f_size = 8;
	unsigned long i;
    for (i = 0; i < nargs; i++) {
         stack_f_size += strlen(args[i]) + 1 + 4; 
     }

    stackptr -= stack_f_size;

    for (; (stackptr % 8) > 0; stackptr--) {
         }


    int arg_string_loc = (int) stackptr + 4 + ((nargs + 1) * 4); ///
    copyout((void *) & nargs, (userptr_t) stackptr, (size_t) 4); //


	for (i = 0; i < nargs; i++) {
    	copyout((void *) & arg_string_loc, (userptr_t) (stackptr + 4 + (4 * i)), (size_t) 4);
       	copyoutstr(args[i], (userptr_t) arg_string_loc, (size_t) strlen(args[i]), NULL); 
       	arg_string_loc += strlen(args[i]) + 1; 
  	}
 
	int *null = NULL;
    copyout((void *) &null, (userptr_t) (stackptr + 4 + (4 * i)), (size_t) 4); 


	md_usermode(nargs /*argc*/, (userptr_t) (stackptr + 4) /*userspace addr of argv*/, 
		stackptr, entrypoint);


 	return -1;
}













