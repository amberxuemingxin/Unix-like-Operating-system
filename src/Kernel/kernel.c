#include <unistd.h> // STDIN_FILENO
#include <signal.h>    // sigaction, sigemptyset, sigfillset, signal
#include <ucontext.h>  // getcontext, makecontext, setcontext, swapcontext
#include <valgrind/valgrind.h> // VALGRIND_STACK_REGISTER

#include "kernel.h"
#include "shell.h"
#include "logger.h"

/* Define macros for signals*/
#define S_SIGSTOP 0
#define S_SIGCONT 1
#define S_SIGTERM 2

int ticks = 0;
pcb_t *foreground;

// extern ucontext_t main_context;
extern ucontext_t scheduler_context;


void set_stack(stack_t *stack)
{
    void *sp = malloc(SIGSTKSZ);
    VALGRIND_STACK_REGISTER(sp, sp + SIGSTKSZ);

    *stack = (stack_t) { .ss_sp = sp, .ss_size = SIGSTKSZ };
}

void make_context(ucontext_t *ucp,  void (*func)(), char *argv[])
{
    getcontext(ucp);

    sigemptyset(&ucp->uc_sigmask);
    set_stack(&ucp->uc_stack);
    ucp->uc_link = func == schedule ? &scheduler_context : NULL;

    makecontext(ucp, func, 1, argv);
}

pcb_t *k_shell_create() {
    pcb_t *shell = (pcb_t *)malloc(sizeof(pcb_t));
    shell->process = "shell";
    shell->fd0 = STDIN_FILENO;
    shell->fd1 = STDOUT_FILENO;
    shell->pid = 1;
    shell->ppid = 1;
    shell->pgid = 1;
    shell->status = RUNNING_P;
    shell->priority = -1;
    shell->ticks = -1;
    shell->children = NULL;

    foreground = shell;

    char *shell_args[2] = {"shell", NULL};
    make_context(&(shell->context), shellLoop, shell_args);
    
    log_events(CREATE, ticks, shell->pid, shell->priority, shell->process);

    return shell;
}

void k_process_create(pcb_t *parent) {
}

int k_process_kill(pcb_t *process, int signal) {
    // stop the process
    if (signal == S_SIGSTOP) {
        process->status = STOPPED;
    }
    return 0;
}

void k_process_cleanup(pcb_t *process) {
    return;
}