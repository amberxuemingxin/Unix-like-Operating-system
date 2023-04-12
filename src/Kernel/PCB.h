#ifndef PCB_HEADER
#define PCB_HEADER

#include <sys/types.h>
#include <ucontext.h> 
#include <stdlib.h>
#include "parser.h"

#define RUNNING_P 0
#define STOPPED_P 1
#define EXITED_P 2
#define ZOMBIED_P 3
#define ORPHANED_P 4
#define BLOCKED_P 5

typedef struct pcb_def
{
    // determine the kind of the process
    char *process;

    // file descriptor for stdin
    int fd0;
    // file descriptor for stdout
    int fd1;

    // process ID
    pid_t pid;
    // parent process ID
    pid_t ppid;
    // process group ID
    pid_t pgid;
    // parent struct
    struct pcb_def *parent;

    struct pcb_def *next;

    // status of the process (RUNNING, BLOCKED, STOPPED, ZOMBIED)
    int status;
    // priority level of the process
    int priority;
    // timing the process
    int ticks;
    // num of process that blocks
    int num_blocks;

    // child process management
    struct pcb_def *children;
    // after exiting the process, if it's being waited on
    bool waited;

    // context of the process
    ucontext_t context;
} pcb_t;

void free_pcb(pcb_t *p);

#endif
