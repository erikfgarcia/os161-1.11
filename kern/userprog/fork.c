/*
fork

OS/161 Reference Manual

Name

fork - copy the current process

Library

Standard C Library (libc, -lc)

Synopsis

#include <unistd.h>
pid_t
fork(void);

Description

fork duplicates the currently running process. The two copies are
identical, except that one (the "new" one, or "child"), has a new,
unique process id, and in the other (the "parent") the process id is
unchanged.

The process id must be greater than 0.

The two processes do not share memory or open file tables; this state is
copied into the new process, and subsequent modification in one process
does not affect the other.

However, the file handle objects the file tables point to are shared,
so, for instance, calls to lseek in one process can affect the other.

Return Values

On success, fork returns twice, once in the parent process and once in
the child process. In the child process, 0 is returned. In the parent
process, the process id of the new child process is returned.

On error, no new process is created, fork only returns once, returning
-1, and errno[1] is set according to the error encountered.

Errors

The following error codes should be returned under the conditions given.
Other error codes may be returned for other errors not mentioned here.

�� EAGAIN Too many processes already exist. ENOMEM Sufficient virtual
memory for the new process was not available.

*/


