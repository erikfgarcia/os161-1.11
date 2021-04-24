/*

*/

#include <types.h>
#include <lib.h>
#include <types.h>
#include <pid.h>
#include <machine/spl.h>

#define PID_FREE   0 //process can be recycled
#define PID_PARENT 1 //process has not exited, but the parent has exited
#define PID_EXITED 2 //process has exited, but the parent has not exited or waited on this pid
#define PID_NEW    3 //neither the process nor its parent have exited

// this pid is not available 
struct pid_clist {
    pid_t pid;
    int status;
    struct pid_clist *next;
};

// this pid has been recycled it can be reused 
struct pid_list {
    pid_t pid;
    struct pid_list *next;
};


unsigned int unused_pids = MIN_PID;//we start from here with pids  
struct pid_list * recycled_pids; //
struct pid_clist * unavailable_pids;// these are used 

/*
get a pid for a new process
*/
pid_t new_pid() {
    int spl = splhigh();

    if (recycled_pids == NULL) {// we need to create a new pid 
        
	struct pid_clist *new_entry = kmalloc(sizeof(struct pid_clist));
        new_entry->pid = unused_pids;
        new_entry->status = PID_NEW;
        new_entry->next = unavailable_pids;
        unavailable_pids = new_entry;
        unused_pids += 1;
        splx(spl);

        return (unused_pids - 1);
    } else {// we can one the recycled pids 
        struct pid_list *first = recycled_pids;
        recycled_pids = recycled_pids->next;

        struct pid_clist *new_entry = kmalloc(sizeof(struct pid_clist)); 
        new_entry->pid = first->pid;
        new_entry->status = PID_NEW;
        new_entry->next = unavailable_pids;
        unavailable_pids = new_entry;
        kfree(first);
        splx(spl);
 
        return (new_entry->pid);
    }
}

void pid_change_status(pid_t x, int and_mask) {
    int spl = splhigh();
 
    assert(unavailable_pids != NULL);
    if (unavailable_pids->pid == x) {
        unavailable_pids->status &= and_mask;

        if (unavailable_pids->status == PID_FREE) {
            //add pid to recycled_pids 
            struct pid_list *new_entry = kmalloc(sizeof(struct pid_list));
            new_entry->pid = x;
            new_entry->next = recycled_pids;
            recycled_pids = new_entry;
            //remove pid from unavailable_pids 
            struct pid_clist *temp = unavailable_pids;
            unavailable_pids = unavailable_pids->next;
            kfree(temp);
 
        }
    } else {
        int found = 0;
        struct pid_clist *p = unavailable_pids;
        while (p->next != NULL) {
            if (p->next->pid == x) {
                found = 1;
                p->next->status &= and_mask;
                      if (p->next->status == PID_FREE) {
                    //add pid to recycled_pids list
                    struct pid_list *new_entry = kmalloc(sizeof(struct pid_list));
                    new_entry->pid = x;
                    new_entry->next = recycled_pids;
                    recycled_pids = new_entry;
                    //remove pid from unavailable_pids list
                    struct pid_clist *temp = p->next;
                    p->next = p->next->next;
                    kfree(temp);
                         }
            }
            p = p->next;
        }
        assert(found);
    }
    splx(spl);
}

/*
indicates that the process's parent has exited, if that is the case the pid doesn't need to be saved 
when the process exit so it if can be recycled
*/
void pid_parent_done(pid_t x) {
    pid_change_status(x, PID_PARENT);
}

/*
use to  indicate that the process has exited,
 it can be recycled
*/
void pid_process_exit(pid_t x) {
    pid_change_status(x, PID_EXITED);
}

/*
recycles the pid; used when a thread with no parent exits
*/
void pid_free(pid_t x) {
    pid_change_status(x, PID_FREE);
}


/*
checks if pid is in use
*/
int pid_claimed(pid_t x) {
    int spl = splhigh();
    struct pid_clist *p;
    for (p = unavailable_pids; p != NULL; p = p->next) {
        if (p->pid == x) {
            splx(spl);
            return 1;
        }
    }
    splx(spl);
    return 0;
}


