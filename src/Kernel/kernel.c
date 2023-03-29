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
extern node *active_node;
extern bool idle;
extern char *log_name;

void idle_process()
{
    sigset_t mask;
    sigemptyset(&mask);
    sigsuspend(&mask);
}

/*
 * sets the stack for a context
 * @param stack, a pointer to the stack_t struct that contaisn the new stack
 */
void set_stack(stack_t *stack)
{
    void *sp = malloc(SIGSTKSZ);
    VALGRIND_STACK_REGISTER(sp, sp + SIGSTKSZ);

    *stack = (stack_t){.ss_sp = sp, .ss_size = SIGSTKSZ};
}

void make_context(ucontext_t *ucp, void (*func)(), char *argv[])
{
    getcontext(ucp);

    sigemptyset(&ucp->uc_sigmask);
    set_stack(&ucp->uc_stack);
    ucp->uc_link = func == schedule ? &scheduler_context : NULL;

    makecontext(ucp, func, 1, argv);
}

void k_foreground_process(pid_t pid)
{
    node *n = search_in_scheduler(pid);

    if (n == NULL)
    {
        perror("Pid not found\n");
        exit(EXIT_FAILURE);
    }

    active_node = n;

    /* block the shell if it's not shell */
    pcb_t *p = (pcb_t *)n->payload;
    if (p->pid != 1)
    {
        p->parent->status = BLOCKED_P;
        log_events(BLOCKED, ticks, p->parent->pid, p->parent->priority, p->parent->process);
    }
}

/*
 * unblock the parent that is being blocked and make it running again
 */
void k_unblock(pid_t ppid)
{
    node *parent = search_in_scheduler(ppid);

    if (parent == NULL)
    {
        perror("Parent not found\n");
        exit(EXIT_FAILURE);
    }

    pcb_t *parent_p = (pcb_t *)parent->payload;
    if (parent_p->status == BLOCKED_P)
    {
        parent_p->status = RUNNING_P;

        log_events(UNBLOCKED, ticks, parent_p->pid, parent_p->priority, parent_p->process);
    }
}

/*
 * create a new shell process
 * @returns a pcb_t pointer to the newly created process
 */
pcb_t *k_shell_create()
{
    pcb_t *shell = (pcb_t *)malloc(sizeof(pcb_t));
    shell->process = "shell";
    shell->fd0 = STDIN_FILENO;
    shell->fd1 = STDOUT_FILENO;
    shell->pid = 1;
    shell->ppid = 1;
    shell->pgid = 1;
    shell->parent = NULL;
    shell->status = RUNNING_P;
    shell->priority = -1;
    shell->ticks = -1;
    shell->children = init_queue();
    shell->zombies = init_queue();
    shell->waited = false;

    char *shell_args[2] = {"shell", NULL};
    make_context(&(shell->context), shell_loop, shell_args);

    // update max pid
    max_pid = shell->pid;

    // initialize a time stamp
    log_name = time_stamp();

    log_events(CREATE, ticks, shell->pid, shell->priority, shell->process);

    // add shell process into scheduler's ready queue
    node *shell_node = init_node(shell);
    add_to_scheduler(shell_node);

    return shell;
}

/*
 * create a new process under a parent
 * @param parent, the parent that the new process that will create under
 * @returns a pcb_t pointer p to the newly created process
 */
pcb_t *k_process_create(pcb_t *parent)
{
    pcb_t *p = (pcb_t *)malloc(sizeof(pcb_t));
    // process name will be assigned later
    p->fd0 = STDIN_FILENO;
    p->fd1 = STDOUT_FILENO;
    p->pid = max_pid + 1;
    p->ppid = parent->pid;
    p->pgid = parent->pgid;
    p->parent = parent;
    p->status = RUNNING_P;
    p->priority = 0;
    p->ticks = -1;
    p->children = init_queue();
    p->zombies = init_queue();
    p->waited = false;

    // add this process to the children queue
    node *n = init_node(p);

    add_node(parent->children, n);

    add_to_scheduler(n);

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
    pcb_t *active_process = (pcb_t *)active_node->payload;

    // stop the process
    if (signal == S_SIGSTOP)
    {
        process->status = STOPPED;
        log_events(STOPPED, ticks, process->pid, process->priority, process->process);

        if (process == active_process)
        {
            k_unblock(process->parent->pid);
        }
    }
    return 0;
}

void k_process_cleanup(pcb_t *process)
{
    return;
}