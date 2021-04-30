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
	warn("SIMPLE EXECV\n");

	execv("/testbin/add", add_argv);	

	return 0;
}
