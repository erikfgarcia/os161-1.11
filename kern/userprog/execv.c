



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
 
 
 
 return -1;
 }


