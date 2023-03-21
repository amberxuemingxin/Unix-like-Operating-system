#include <sys/types.h>
#include <ucontext.h> 
#include <stdlib.h>
#include "parser.h"

#define RUNNING 0
#define BLOCKED 1
#define STOPPED 2
#define ZOMBIED 3

typedef struct pcb
{
    const char *fd0;
    const char *fd1;
    pid_t pid;
    pid_t ppid;
    int status;
    int priority;
    ucontext_t *context;
} pcb_t;

pcb_t *pcb_spawn(ucontext_t *context, int pid, int ppid, int priority, struct parsed_command *cmd);

void free_pcb(pcb_t *p);
