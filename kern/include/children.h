
#ifndef _CHILDREN_H_
#define _CHILDREN_H_

#include <pid.h>

/*
 *This keeps record of children 
 *
 */
struct children{
    pid_t pid;
    volatile int finished; //failsafe for if someone uses -1 as an exit code
    int exit_code;
    struct children *next;
};
#endif 
