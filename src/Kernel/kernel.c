#include <unistd.h> // STDIN_FILENO
#include <signal.h>    // sigaction, sigemptyset, sigfillset, signal
#include <ucontext.h>  // getcontext, makecontext, setcontext, swapcontext
#include <valgrind/valgrind.h> // VALGRIND_STACK_REGISTER

#include "kernel.h"
#include "PCB.h"
#include "shell.h"

ucontext_t mainContext;
ucontext_t schedulerContext;

void setStack(stack_t *stack)
{
    void *sp = malloc(SIGSTKSZ);
    VALGRIND_STACK_REGISTER(sp, sp + SIGSTKSZ);

    *stack = (stack_t) { .ss_sp = sp, .ss_size = SIGSTKSZ };
}

void makeContext(ucontext_t *ucp,  void (*func)(), char *argv[])
{
    getcontext(ucp);

    sigemptyset(&ucp->uc_sigmask);
    setStack(&ucp->uc_stack);
    ucp->uc_link = &schedulerContext;

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
    shell->zombies = NULL;

    char *shellArgs[2] = {"shell", NULL};
    makeContext(shell->context, shellLoop, shellArgs);

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