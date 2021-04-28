#ifndef _PID_H_
#define _PID_H_

#define MINIMUM_N_PID 100 //the minimum pid number

#include <types.h>

pid_t new_pid();
void pid_process_exit(pid_t pid);
int pid_is_used (pid_t pid);
void pid_parent_exit(pid_t pid);
void pid_free(pid_t pid);

#endif
