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
    int status;
    int priority;
    ucontext_t *ucp;
} pcb_t;

/*(U) forks a new thread that retains most of the attributes of the parent thread 
(see k_process_create). Once the thread is spawned, it executes the function 
referenced by func with its argument array argv.
fd0 is the file descriptor for the input file, and fd1 is the file descriptor for 
the output file. It returns the pid of the child thread on success, or -1 on error.*/
pid_t p_spawn(void (*func)(), char *argv[], int fd0, int fd1);

void p_kill(pid_t pid, int sig);

void p_waitpid(pid_t pid, int *wstatus. bool nohang);

void p_exit();

int p_nice(pid_t pid, int priority);

void p_sleep(unsigned int ticks);
