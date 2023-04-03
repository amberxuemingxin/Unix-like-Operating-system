#include <unistd.h>            // STDIN_FILENO
#include <signal.h>            // sigaction, sigemptyset, sigfillset, signal
#include <ucontext.h>          // getcontext, makecontext, setcontext, swapcontext
#include <valgrind/valgrind.h> // VALGRIND_STACK_REGISTER

#include "kernel.h"
#include "shell.h"
#include "logger.h"
#include "scheduler.h"

// Define macros for signals
#define S_SIGSTOP 0
#define S_SIGCONT 1
#define S_SIGTERM 2

// global variables
int ticks = 0;
pid_t max_pid = 0;

// extern ucontext_t main_context;
extern ucontext_t scheduler_context;
extern pcb_t *active_process;
extern bool idle;

void idle_process()
{
    while (1) {
        sigset_t mask;
        sigemptyset(&mask);
        sigsuspend(&mask);
    }
}

/*
 * helper that sets the stack for a context
 * @param stack, a pointer to the stack_t struct that contaisn the new stack
 */
void set_stack(stack_t *stack)
{
    void *sp = malloc(SIGSTKSZ);
    VALGRIND_STACK_REGISTER(sp, sp + SIGSTKSZ);

    *stack = (stack_t){.ss_sp = sp, .ss_size = SIGSTKSZ};
}

void make_context(ucontext_t *ucp, void (*func)(), int arg_num, char *argv[])
{
    getcontext(ucp);

    sigemptyset(&ucp->uc_sigmask);
    set_stack(&ucp->uc_stack);
    ucp->uc_link = func == schedule ? &scheduler_context : NULL;

    makecontext(ucp, func, arg_num, argv);
}

void k_foreground_process(pid_t pid)
{
    pcb_t *p = search_in_scheduler(pid);

    if (p == NULL)
    {
        perror("Pid not found\n");
        exit(EXIT_FAILURE);
    }

    active_process = p;

    /* block the shell if it's not shell */
    if (p->pid != 1)
    {
        p->parent->status = BLOCKED_P;
        log_events(BLOCKED, ticks, p->parent->pid, p->parent->priority, p->parent->process);
    }
}

void k_block(pcb_t *parent) {
    parent->status = BLOCKED_P;
    remove_from_scheduler(parent);
    log_events(BLOCKED, ticks, parent->pid, parent->priority, parent->process);
}

/*
 * unblock the parent that is being blocked and make it running again
 */
void k_unblock(pcb_t *parent)
{
    if (parent->status == BLOCKED_P)
    {
        parent->status = RUNNING_P;
        add_to_scheduler(parent);
        log_events(UNBLOCKED, ticks, parent->pid, parent->priority, parent->process);
    } else {
        perror("Parent is not blocked!");
    }
}

/*
 * create a new process under a parent
 * @param parent, the parent that the new process that will create under
 * @returns a pcb_t pointer p to the newly created process
 */
pcb_t *k_process_create(pcb_t *parent, bool is_shell)
{
    pcb_t *p = (pcb_t *)malloc(sizeof(pcb_t));
    // process name will be assigned later
    p->fd0 = STDIN_FILENO;
    p->fd1 = STDOUT_FILENO;
    p->pid = is_shell ? 1 : max_pid + 1;
    p->ppid = is_shell ? 1 : parent->pid;
    p->pgid = is_shell ? 1: parent->pgid;
    p->parent = parent;
    p->status = RUNNING_P;
    p->priority = is_shell ? -1 : 0;
    p->ticks = -1;
    p->children = NULL;
    p->zombies = NULL;
    p->next = NULL;
    p->waited = false;

    // add this process to the children queue
    if (!is_shell) {
        pcb_t *child = parent->children;
        while (child) {
            child = child->next;
        }

        if (child != NULL) {
            child->next = p;
        } else {
            parent->children = p;
        }
    }

    // update max pid
    max_pid = p->pid;

    return p;
}
/*
 * kill the designated process
 * @param process, the process needs to be kills
 * @param signal, the signal intended
 */
int k_process_kill(pcb_t *process, int signal)
{

    // stop the process
    if (signal == S_SIGSTOP)
    {
        process->status = STOPPED;
        log_events(STOPPED, ticks, process->pid, process->priority, process->process);

        if (process == active_process)
        {
            k_unblock(process->parent);
        }
    }
    return 0;
}

void k_process_cleanup(pcb_t *process)
{
    return;
}