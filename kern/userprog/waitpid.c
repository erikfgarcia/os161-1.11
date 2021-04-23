
/*

waitpid

OS/161 Reference Manual

Name

waitpid - wait for a process to exit

Library

Standard C Library (libc, -lc)

Synopsis

#include <sys/wait.h>
pid_t
waitpid(pid_t pid, int *status, int options);

Description

Wait for the process specified by pid to exit, and return its exit code
in the integer pointed to by status. If that process has exited already,
waitpid returns immediately. If that process does not exist, waitpid
fails.

What it means for a process to move from "has exited already" to "does
not exist", and when this occurs, is something you must decide.

If process P is "interested" in the exit code of process Q, process P
should be able to find out that exit code by calling waitpid, even if Q
exits somewhat before the time P calls waitpid. As described under
_exit()[1], precisely what is meant by "interested" is up to you.

You might implement restrictions or requirements on who may wait for
which processes, like Unix does. You might also add a system call for
one process to express interest in another process's exit code. If you
do this, be sure to write a man page for the system call, and discuss
the rationale for your choices therein in your design document.

Note that in the absence of restrictions on who may wait for what, it is
possible to set up situations that may result in deadlock. Your system
must (of course) in some manner protect itself from these situations,
either by prohibiting them or by detecting and resolving them.

In order to make the userlevel code that ships with OS/161 work, assume
that a parent process is always interested in the exit codes of its
child processes generated with fork()[2], unless it does something
special to indicate otherwise.

The options argument should be 0. You are not required to implement any
options. (However, your system should check to make sure that options
you do not support are not requested.)

If you desire, you may implement the Unix option WNOHANG; this causes
waitpid, when called for a process that has not yet exited, to return 0
immediately instead of waiting.

You may also make up your own options if you find them helpful. However,
please, document anything you make up.

Return Values

waitpid returns the process id whose exit status is reported in status.
In OS/161, this is always the value of pid.

If you implement WNOHANG, and WNOHANG is given, and the process
specified by pid has not yet exited, waitpid returns 0.

(In Unix, but not OS/161, you can wait for any of several processes by
passing magic values of pid, so this return value can actually be
useful.)

On error, -1 is returned, and errno is set to a suitable error code for
the error condition encountered.

Errors

The following error codes should be returned under the conditions given.
Other error codes may be returned for other errors not mentioned here.

�� EINVAL The options argument requested invalid or unsupported options.
EFAULT The status argument was an invalid pointer.

The traditional Unix error code for "no such process" is ESRCH, although
this code is not defined in the OS/161 base system.

*/

  
        