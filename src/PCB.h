#include <sys/types.h>
#include <ucontext.h> 

#define RUNNING 0
#define BLOCKED 1
#define STOPPED 2
#define ZOMBIED 3
#define ORPHANED 4

typedef struct pcb
{
    char *fd0;
    char *fd1;
    pid_t pid;
    pid_t ppid;
    pid_t *cpid;
    int jid;
    int status;
    int priority;
    ucontext_t *ucp;
} pcb_t;
