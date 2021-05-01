/*
 * simpleexit - test fork(), exit, getpid(), getppid().
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
	int status;
	pid = fork();
      
	if(pid > 0) {
		// parent

		printf("PARENT of %d, PID=%d\n", pid, getpid());
		if (waitpid(pid, &status, 0)<0) {
       		printf("ERROR: waitpid\n");
      	}
		else {
			printf("PARENT waited on %d [exit %d], PID=%d\n", pid, status, getpid());
		}


	}else if(pid == 0) {
		// child
		int i;	
		int max = 1000000;
		int period = max / 10;	

		printf("CHILD of %d, PID=%d\n", getppid(), getpid());

		printf("CHILD: sleeping\n");
		for(i=0; i<max; i++) {
			if(i%period == 0)
				printf(" ==> %d/10 done\n", i/period);
		}
		printf("CHILD: awake, exit\n");
		exit(37);
		
	}else {
		// error
		printf("ERROR: fork\n");
	}		

	return 0;
}
