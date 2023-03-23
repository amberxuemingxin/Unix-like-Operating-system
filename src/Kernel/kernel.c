#include <unistd.h> // STDIN_FILENO
#include <signal.h>    // sigaction, sigemptyset, sigfillset, signal
#include <ucontext.h>  // getcontext, makecontext, setcontext, swapcontext
#include <valgrind/valgrind.h> // VALGRIND_STACK_REGISTER

#include "kernel.h"
#include "shell.h"
#include "logger.h"

int ticks = 0;
ucontext_t main_context;
ucontext_t scheduler_context;

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
    ucp->uc_link = &scheduler_context;

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
    shell->status = RUNNING;
    shell->priority = -1;
    shell->children = NULL;

    char *shellArgs[2] = {"shell", NULL};
    make_context(&(shell->context), shellLoop, shellArgs);
    
    log_events(CREATE, ticks, shell->pid, shell->priority, shell->process);

    return shell;
}

void k_process_create(pcb_t *parent) {
}

int k_process_kill(pcb_t *process, int signal) {
    return 0;
}

void k_process_cleanup(pcb_t *process) {
    return;
}