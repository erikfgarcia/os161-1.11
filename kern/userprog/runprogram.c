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

	// check for additional arguments
	if(nargs == 1) {
		// no additional arguments

		/* Warp to user mode. */
		md_usermode(0 /*argc*/, NULL /*userspace addr of argv*/,
		    stackptr, entrypoint);
	}
	/*else {
		// ATTEMPT 1


		// has additional arguments
		int i;
		vaddr_t argv_ptrs[nargs];		// str pointers on stack
		int str_size;
		int str_size_aligned;
		int copy_err;

		// copy additional arg strings to stack
		for(i=nargs-1; i>0; i--) {
			// calc string size and aligned string size
			str_size = strlen(args[i]);
			str_size_aligned = (str_size/4 + 1)*4;

			// adjust and copy to stack
			stackptr -= str_size_aligned;
			copy_err = copyoutstr(args[i], (userptr_t)stackptr, str_size+1, NULL);

			// check for copy error
			if(copy_err) {
				// copy error detected
				return copy_err;
			}

			// record string arg stack address
			argv_ptrs[i-1] = stackptr;
		}

		argv_ptrs[nargs-1] = (vaddr_t)NULL;

		// copy string arg addresses to stack
		for(i=nargs-1; i>=0; i++) {
			// adjust and copy to stack
			stackptr -= 4;
			copy_err = copyout(&(argv_ptrs[i]), (userptr_t)stackptr, 4);

			// check for copy error
			if(copy_err) {
				// copy error detected
				return copy_err;
			}
		}

		// to user mode
		md_usermode(nargs-1, (userptr_t)stackptr, stackptr, entrypoint);	
	}*/
	else {
		// ATTEMPT 2
	
		//kprintf("\nTEST: #args=%lu\n", nargs);

		unsigned long i;
		size_t len;
		size_t stackoffset = 0;	
		vaddr_t argv_ptrs[nargs];
		int copy_err;
		
		// copy string args onto stack
		for(i=1; i<nargs; i++) {
			kprintf("\nTEST: arg=%s\n\n", args[i]);

			len = strlen(args[i]) + 1;
			stackoffset += len;
			argv_ptrs[i-1] = stackptr - stackoffset;
			copy_err = copyout(args[i], (userptr_t)argv_ptrs[i-1], len);

			if(copy_err) {
				// copy error
				kprintf("\nTEST: copy error 1\n");
			}
		}
		// NULL terminating pointer
		argv_ptrs[nargs-1] = 0;

		// adjust stack pointer
		stackoffset += sizeof(vaddr_t) * nargs;
		stackptr = stackptr - stackoffset - ((stackptr-stackoffset)%8);
		copy_err = copyout(argv_ptrs, (userptr_t)stackptr, sizeof(vaddr_t)*nargs);

		if(copy_err) {
			// copy error
			kprintf("\nTEST: copy error 2\n");
		}

		//kprintf("\nTEST: to user mode\n");
		
		// return to user mode
		md_usermode(nargs-1, (userptr_t)stackptr, stackptr, entrypoint);
	}
	
	/* md_usermode does not return */
	panic("md_usermode returned\n");
	return EINVAL;
}

