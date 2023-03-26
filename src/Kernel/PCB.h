#ifndef PCB_HEADER
#define PCB_HEADER

#include <sys/types.h>
#include <ucontext.h> 
#include <stdlib.h>
#include "parser.h"
#include "queue.h"

#define RUNNING_P 0
#define BLOCKED_P 1
#define STOPPED_P 2
#define ZOMBIED_P 3
#define EXITED_P 4

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

    // status of the process (RUNNING, BLOCKED, STOPPED, ZOMBIED)
    int status;
    // priority level of the process
    int priority;
    // timing the process
    int ticks;

    // child process management
    queue *children;
    // zombie process management
    queue *zombies;
    // after exiting the process, if it's being waited on
    bool waited;

    // context of the process
    ucontext_t context;
} pcb_t;

#endif
