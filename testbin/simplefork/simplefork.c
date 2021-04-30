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
       //  int x;
	pid = fork();
      
	if(pid > 0) {
		// parent
		warn("PARENT of %d, PID=%d\n", pid, getpid());
		if (waitpid(pid, &x, 0)<0) {
                         warn("waitpid");
                 }

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
