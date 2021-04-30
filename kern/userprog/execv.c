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

�� ENODEV The device prefix of program did not exist.
 ENOTDIR A non-final component of program was not a directory.
 ENOENT program did not exist.
 EISDIR program is a directory. 
 ENOEXEC program is not in a recognizable executable file format, was for the wrong platform, or
contained invalid fields. 
ENOMEM Insufficient virtual memory is available.
 E2BIG The total size of the argument strings is too large.
 EIO A hard I/O error occurred. 
EFAULT One of the args is an invalid
pointer.


*/

int sys_execv(const char *progname, char **args){


    assert(curthread->t_vmspace != NULL);

   if (strlen(progname) < 1) 
        return EFAULT;   

//copyin the arguments from user space to kernel space ( the programname and args)
    
   // we need to calculate the number of args
    int nargs = 0;
    int i = 0;
    while (args[i] != NULL) {
        nargs++;
        i++;
    }
    

    // get space and copy  progname
    char* progname2 = kmalloc(sizeof (char) * (strlen(progname)+1));
   if(progname2==NULL)
	return ENOMEM;

    copyinstr((const_userptr_t) progname, progname2, (strlen(progname)+1), NULL);

    // get space for the argumenta and capy the argumest into the kernel
    char** arguments= kmalloc(sizeof (char*) * nargs);
    if(arguments ==NULL)
        return ENOMEM;

    for (i = 0; i < nargs; i++) {
        
        if (strlen(args[i]) < 1) {
            return EFAULT;
        }
        int arg_size = strlen(args[i]);
        arguments[i] = kmalloc(sizeof (char) * (arg_size + 1));
	if(arguments[i]==NULL)
         	return ENOMEM;
        copyinstr((const_userptr_t) args[i], arguments[i], (arg_size + 1), NULL);
    }

////////////////////////////////////////////////// 
  

    struct vnode *v;
    vaddr_t entrypoint, stackptr;
    int result;

    /* Open the file. */
    result = vfs_open(progname2, O_RDONLY, &v);
    if (result) {
        return result;
    }
 
   // destory current address space
    as_destroy(curthread->t_vmspace);
    curthread->t_vmspace = NULL;
  
    assert(curthread->t_vmspace == NULL);

   
   /* Create a new address space. */
    curthread->t_vmspace = as_create();
    if (curthread->t_vmspace == NULL) {
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

    /* Done with the file now*/
    vfs_close(v);



////////////////////////////////////////////

    /* calculate the size of the stack  */

    int stack_f_size  = 8; //
    for (i = 0; i < nargs; i++) {
        stack_f_size  += strlen(arguments[i]) + 1 + 4; //length of each argument plus the pointer
    }

    /* Decide the stackpointer address */
    stackptr -= stack_f_size;
    for (; (stackptr % 8) > 0; stackptr--) { }

    // copy the arguments on the stack 
    int arg_string_loc = (int) stackptr + 4 + ((nargs + 1) * 4); //begining 
    copyout((void *) & nargs, (userptr_t) stackptr, (size_t) 4); 
    for (i = 0; i < nargs; i++) {
        copyout((void *) & arg_string_loc, (userptr_t) (stackptr + 4 + (4 * i)), (size_t) 4); 
        copyoutstr(arguments[i], (userptr_t) arg_string_loc, (size_t) strlen(arguments[i]), NULL); 
        arg_string_loc += strlen(arguments[i]) + 1; //next iteration
    }
   //place a  null in last position
    int *null = NULL;
    copyout((void *) & null, (userptr_t) (stackptr + 4 + (4 * i)), (size_t) 4); 

    // free  memory used for arguments 
    kfree(progname2);

    //free the arguments
    for (i = 0; i < nargs; i++) {
        kfree(arguments[i]);
    }
    
    kfree(arguments);
 
    md_usermode(nargs /*argc*/, (userptr_t) (stackptr+4) /*userspace addr of argv*/, stackptr, entrypoint);

    /* md_usermode does not return */
    panic("md_usermode returned\n");
    return EINVAL;
}





