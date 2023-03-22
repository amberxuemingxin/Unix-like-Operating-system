#ifndef PCB_HEADER
#define PCB_HEADER

#include <sys/types.h>
#include <ucontext.h> 
#include <stdlib.h>
#include "parser.h"

#define RUNNING 0
#define BLOCKED 1
#define STOPPED 2
#define ZOMBIED 3

typedef struct pcb_def
{
    const char *fd0;
    const char *fd1;
    pid_t pid;
    pid_t ppid;
    int status;
    int priority;
    ucontext_t *context;
} pcb_t;

#endif