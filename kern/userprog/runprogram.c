/*
 * Sample/test code for running a user program.  You can use this for
 * reference when implementing the execv() system call. Remember though
 * that execv() needs to do more than this function does.
 */

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
 * Load program "progname" and start running it in usermode.
 * Does not return except on error.
 *
 * Calls vfs_open on progname and thus may destroy it.
 */
/*
 * args contains program name and args in array of strings
 * nargs is total number of command-line args (progname + additional-args)
 */
int
runprogram(char *progname, unsigned long nargs, char **args)
{


	struct vnode *v;
	vaddr_t entrypoint, stackptr;
	int result;

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




	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


     int stack_f_size = 8;
     unsigned long i;
     for (i = 0; i < nargs; i++) {
 
         stack_f_size += strlen(args[i]) + 1 + 4; 
     }

    stackptr -= stack_f_size;

     for (; (stackptr % 8) > 0; stackptr--){ } 


    int arg_string_loc = (int) stackptr + 4 + ((nargs + 1) * 4); ///
     copyout((void *) & nargs, (userptr_t) stackptr, (size_t) 4); //


for (i = 0; i < nargs; i++) {
         copyout((void *) & arg_string_loc, (userptr_t) (stackptr + 4 + (4 * i)), (size_t) 4);
         copyoutstr(args[i], (userptr_t) arg_string_loc, (size_t) strlen(args[i]), NULL); 
         arg_string_loc += strlen(args[i]) + 1; 
     }
 
 int *null = NULL;
     copyout((void *) &null, (userptr_t) (stackptr + 4 + (4 * i)), (size_t) 4); 


md_usermode(nargs /*argc*/, (userptr_t) (stackptr + 4) /*userspace addr of argv*/, stackptr, entrypoint);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/* md_usermode does not return */
	panic("md_usermode returned\n");
	return EINVAL;
}


