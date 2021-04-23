#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */

int sys_reboot(int code);
//new syscalls 
void sys__exit(int exitcode);
pid_t sys_waitpid(pid_t pid, int *status, int options);
//int sys_execv(const char *program, char **args); 
//pid_t sys_getpid(void);
//pid_t sys_gettpid(void);
//pid_t sys_fork(void);

#endif /* _SYSCALL_H_ */
