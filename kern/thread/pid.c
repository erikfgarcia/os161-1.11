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
struct pid_asigned_list {
    pid_t pid;
    int status;
    struct pid_asigned_list *next;
};

// this pid has been recycled it can be reused 
struct pid_recycle_list {
    pid_t pid;
    struct pid_recycle_list *next;
};


unsigned int unused_pids = MINIMUM_N_PID;//we start from here with pids number  
struct pid_recycle_list * recycled_pids; // these can be reasign 
struct pid_asigned_list * unavailable_pids;// these are used 

/*
get a pid for a new process
*/
pid_t new_pid() {
    int spl = splhigh();

    if (recycled_pids == NULL) {//this creates and assigns new pids   
        
	struct pid_asigned_list *new_entry = kmalloc(sizeof(struct pid_asigned_list));
        new_entry->pid = unused_pids;// 
        new_entry->status = PID_NEW;// 
        new_entry->next = unavailable_pids;
        unavailable_pids = new_entry;
        unused_pids++;
        splx(spl);
        return (unused_pids-1);

    } else {// we can use one  of the recycled pids 
        struct pid_recycle_list *temp = recycled_pids;
        recycled_pids = recycled_pids->next;

        struct pid_asigned_list *new_entry = kmalloc(sizeof(struct pid_asigned_list)); 
        new_entry->pid = temp->pid;
        new_entry->status = PID_NEW;
        new_entry->next = unavailable_pids;
        unavailable_pids = new_entry;
        kfree(temp);
        splx(spl);
        return (new_entry->pid);
    }
}

void pid_change_status(pid_t pid, int and_mask) {
    int spl = splhigh();
 
    assert(unavailable_pids != NULL);
    if (unavailable_pids->pid == pid) {
        unavailable_pids->status &= and_mask;//    PID_FREE = 0000 PID_PARENT = 0001 PID_EXITED = 0010 PID_NEW 0011  
                                                 // NEW & PARENT = PARENT, NEW & EXIT = EXIT, PARENT & EXIT = FREE, and the other way around
                                                 // To be recycle the process and its parent have to exit   
        if (unavailable_pids->status == PID_FREE) {
            //add pid to recycled_pids 
            struct pid_recycle_list *new_entry = kmalloc(sizeof(struct pid_recycle_list));
            new_entry->pid = pid;
            new_entry->next = recycled_pids;
            recycled_pids = new_entry;
            //remove pid from unavailable_pids 
            struct pid_asigned_list *temp = unavailable_pids;
            unavailable_pids = unavailable_pids->next;
            kfree(temp);
 
        }
    } else {
        int found = 0;
        struct pid_asigned_list *p = unavailable_pids;
        while (p->next != NULL) {
            if (p->next->pid == pid) {
                found = 1;
                p->next->status &= and_mask;
                      if (p->next->status == PID_FREE) {
                    //add pid to recycled_pids list
                    struct pid_recycle_list *new_entry = kmalloc(sizeof(struct pid_recycle_list));
                    new_entry->pid = pid;
                    new_entry->next = recycled_pids;
                    recycled_pids = new_entry;
                    //remove pid from unavailable_pids list
                    struct pid_asigned_list *temp = p->next;
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
 checks if pid is in use
*/
 int pid_is_used(pid_t pid) {
     int spl = splhigh();
     struct pid_asigned_list *p;
     for (p = unavailable_pids; p != NULL; p = p->next) {
         if (p->pid == pid) {
             splx(spl);
             return 1;
         }
     }
     splx(spl);
     return 0;
 }
 

/*
use to  indicate that the process has exited,
 it can be recycled
*/
void pid_process_exit(pid_t pid) {
    pid_change_status(pid, PID_EXITED);
}


/*
 * indicates that the process's parent has exited, if that is the case the pid doesn't need to be saved 
 * when the process exit so it if can be recycled
 * */
 void pid_parent_exit(pid_t pid) {
     pid_change_status(pid, PID_PARENT);
}


/*
recycles the pid; used when a thread with no parent exits
*/
void pid_free(pid_t pid) {
    pid_change_status(pid, PID_FREE);
}
 


