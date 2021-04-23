/*


This system call works exactly like getpid(), except that it returns the parent process’ pid. Note that,
the getppid() system call is not included in the OS/161 man pages, and you can assume the following
spec:
7getppid() will return the process ID of the parent of the calling process. This will be either the ID of
the process that created this process using fork() (if the parent hasn’t terminated yet), or -1 (if the
parent has already terminated).



*/
