#ifndef _PID_H_
#define _PID_H_

#define MIN_PID 100 //the minimum pid 

#include <types.h>

pid_t new_pid();
void pid_process_exit(pid_t x);
void pid_parent_exit(pid_t x);
void pid_free(pid_t x);
int pid_claimed(pid_t x);

#endif
