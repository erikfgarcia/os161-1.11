/*
 * simpleexecv.c
 *
 *
 * This test should itself run correctly when the basic system calls
 * are complete. It may be helpful for scheduler performance analysis.
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

static char *add_argv[3] = { (char *)"5", (char *)"7", NULL };


int
main()
{
	int pid;
	int status;

	printf("SIMPLE EXECV\n");

	pid = fork();	

	if(pid == 0) {
		printf("CHILD: calling execv\n");
		execv("/testbin/add", add_argv);
	}	
	else if(pid > 0) {
		printf("PARENT: wait\n");
		waitpid(pid, &status, 0);
		printf("PARENT: waited\n");
	}
	else {
		printf("ERROR: fork\n");
	}

	return 0;
}
