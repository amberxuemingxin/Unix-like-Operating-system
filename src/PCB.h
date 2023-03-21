#include <sys/types.h>
#include <ucontext.h> 

#define RUNNING 0
#define BLOCKED 1
#define STOPPED 2
#define ZOMBIED 3

typedef struct pcb
{
    int fd0;
    int fd1;
    pid_t pid;
    pid_t ppid;
    int status;
    int priority;
    ucontext_t *ucp;
} pcb_t;

pcb_t *pcb_spawn(ucontext_t *context, int pid, int ppid, int priority, char **arg);

int free_pcb(pcb_t *p);
