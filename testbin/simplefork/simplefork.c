/*
 * simple_fork - test fork(), getpid(), getppid().
 *
 * This should work correctly when fork is implemented.
 *
 * It should also continue to work after subsequent assignments, most
 * notably after implementing the virtual memory system.
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

/*
 * NEW TEST
 */


int
main(int argc, char *argv[])
{
	int pid;
<<<<<<< HEAD
       //  int x;
=======
	int status;

>>>>>>> fe6731948c644cd77532489b42f507fa0a5d3e4b
	pid = fork();
      
	if(pid > 0) {
		// parent
<<<<<<< HEAD
		warn("PARENT of %d, PID=%d\n", pid, getpid());
		if (waitpid(pid, &x, 0)<0) {
                         warn("waitpid");
                 }

=======
		//warn("PARENT of %d, PID=%d\n", pid, getpid());
		waitpid(pid, &status, 0);
		warn("PARENT waited on %d, PID=%d\n", pid, getpid());
>>>>>>> fe6731948c644cd77532489b42f507fa0a5d3e4b
	}
	else if(pid == 0) {
		// child
		warn("CHILD of %d, PID=%d\n", getppid(), getpid());
	}
	else {
		// error
		warn("ERROR\n");
	}		

	return 0;
}
