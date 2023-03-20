#include <sys/types.h>
#include <ucontext.h> 

#define RUNNING 0
#define BLOCKED 1
#define STOPPED 2
#define ZOMBIED 3
#define ORPHANED 4

typedef struct pcb
{
    char *in_file;
    char *out_file;
    pid_t pid;
    pid_t ppid;
    pid_t *cpid;
    int status;
    int priority;
    ucontext_t *ucp;
} pcb_t;

void p_kill(pid_t pid, int signal);
void p_waitpid();
void p_exit();
