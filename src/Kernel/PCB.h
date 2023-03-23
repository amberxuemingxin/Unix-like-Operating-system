#ifndef PCB_HEADER
#define PCB_HEADER

#include <sys/types.h>
#include <ucontext.h> 
#include <stdlib.h>
#include "parser.h"
#include "queue.h"

#define RUNNING 0
#define BLOCKED 1
#define STOPPED 2
#define ZOMBIED 3

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
    node *children;
    node *zombies;

    // context of the process
    ucontext_t *context;
} pcb_t;

#endif
