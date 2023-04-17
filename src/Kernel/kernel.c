#include <unistd.h>            // STDIN_FILENO
#include <signal.h>            // sigaction, sigemptyset, sigfillset, signal
#include <ucontext.h>          // getcontext, makecontext, setcontext, swapcontext
#include <valgrind/valgrind.h> // VALGRIND_STACK_REGISTER

#include "kernel.h"
#include "shell.h"
#include "logger.h"
#include "scheduler.h"
#include "queue.h"
#include "builtins.h"
#include "jobs.h"

#define STACKSIZE 819200

// global variables
int global_ticks = 0;
pid_t max_pid = 0;

extern ucontext_t scheduler_context;
extern ucontext_t exit_context;
extern ucontext_t idle_context;
extern queue *queue_block;
extern queue *queue_zombie;
extern pcb_t *active_process;
extern job_list *list;
extern bool idle;

void orphan_check(pcb_t *process) {
    children_list *child = process->children;

    while (child) {
        pcb_t *p = search_in_scheduler(child->pid) ? search_in_scheduler(child->pid) : search_in_zombies(child->pid);
        children_list *tmp = child->next;
        if (p == NULL) {
            printf("Can't find child's pid\n");
            return;
        }
        log_events(ORPHAN, global_ticks, p->pid, p->priority, p->process);
        p->status = ORPHANED_P;
        remove_from_scheduler(p);
        k_process_cleanup(p);
        free(child);
        child = tmp;
    }

    process->children = NULL;
}

void idle_process()
{
    while (1) {
        sigset_t mask;
        sigemptyset(&mask);
        sigsuspend(&mask);
    }
}

void exit_process() {
    if (active_process) {
        k_process_kill(active_process, S_SIGTERM);

        active_process->status = ZOMBIED_P;
        log_events(ZOMBIE, global_ticks, active_process->pid, active_process->priority, active_process->process);
        add_process(queue_zombie, active_process);
        remove_from_scheduler(active_process);
        
        // unblock parent here (skip if it's bg)
        if (active_process->parent) {
            if (list->fg_job && list->fg_job->pid != active_process->pid) {
                return;
            }
            k_unblock(active_process->parent);
        }
    }
}

/*
 * helper that sets the stack for a context
 * @param stack, a pointer to the stack_t struct that contaisn the new stack
 */
void set_stack(stack_t *stack)
{
    void *sp = malloc(STACKSIZE);
    VALGRIND_STACK_REGISTER(sp, sp + STACKSIZE);

    *stack = (stack_t){.ss_sp = sp, .ss_size = STACKSIZE};
}

void make_context(ucontext_t *ucp, void (*func)(), int argc, char *argv[])
{
    getcontext(ucp);

    sigemptyset(&ucp->uc_sigmask);
    set_stack(&ucp->uc_stack);
    if (func == schedule) {
        ucp->uc_link = NULL;
    } else if (func == idle_process || func == exit_process) {
        ucp->uc_link = &scheduler_context;
    } else {
        ucp->uc_link = &exit_context;
    }

    switch (argc)
    {
    case 0:
        makecontext(ucp, func, 0);
        break;
    case 1:
        makecontext(ucp, func, 1, argv[0]);
        break;
    case 2:
        makecontext(ucp, func, 2, argv[0], argv[1]);
        break;
    default:
        break;
    }
}

void k_foreground_process(pid_t pid)
{
    pcb_t *p = search_in_scheduler(pid);

    if (p == NULL)
    {
        perror("Pid not found\n");
        exit(EXIT_FAILURE);
    }

    /* block the shell if it's not shell */
    if (p->pid != 1)
    {
        p->parent->status = BLOCKED_P;
        log_events(BLOCKED, global_ticks, p->parent->pid, p->parent->priority, p->parent->process);
    }
}

void k_block(pcb_t *parent) {
    if (parent) {
        parent->num_blocks++;
        /* 0->1, block the process*/
        if (parent->num_blocks == 1) {
            parent->status = BLOCKED_P;
            ready_to_block(parent);

            log_events(BLOCKED, global_ticks, parent->pid, parent->priority, parent->process);
            swapcontext(&active_process->context, &scheduler_context);
        }
    }

}

/*
 * unblock the parent that is being blocked and make it running again
 */
void k_unblock(pcb_t *parent)
{
    if (parent) {
        parent->num_blocks--;
        if (parent->num_blocks == 0) {
            parent->status = RUNNING_P;
            add_to_scheduler(parent);
            remove_process(queue_block, parent);
            log_events(UNBLOCKED, global_ticks, parent->pid, parent->priority, parent->process);
        }
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
    p->ppid = is_shell ? 0 : parent->pid;
    p->pgid = is_shell ? 1: parent->pgid;
    p->parent = parent;
    p->status = RUNNING_P;
    p->ticks = -1;
    p->num_blocks = 0;
    p->children = NULL;
    p->next = NULL;

    // add this process to the children queue
    if (!is_shell) {
        children_list *cur = malloc(sizeof(children_list));
        cur->pid = p->pid;
        cur->next = NULL;

        children_list *child = parent->children;
        children_list *prev = NULL;
        while (child) {
            prev = child;
            child = child->next;
        }

        if (prev != NULL) {
            prev->next = cur;
        } else {
            parent->children = cur;
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
        process->status = STOPPED_P;
        log_events(STOPPED, global_ticks, process->pid, process->priority, process->process);

        if (strcmp(process->process, "sleep") != 0) {
            ready_to_block(process);
        }

        if (process == active_process)
        {
            k_unblock(process->parent);
        }

        return 0;
    } else if (signal == S_SIGTERM) 
    {
        process->status = EXITED_P;
        log_events(EXITED, global_ticks, process->pid, process->priority, process->process);
        orphan_check(process); 

        return 0;
    } else if (signal == S_SIGCONT)
    {
        if (process->status == EXITED_P || process->status == ZOMBIED_P) {
            perror("Can't continue a dead process");
            return -1;
        } else if (process->status != RUNNING_P) {
            process->status = RUNNING_P;
            log_events(CONTINUED, global_ticks, process->pid, process->priority, process->process);
            if (strcmp(process->process, "sleep") != 0) {
                block_to_ready(process);
            }
        }  
    } else if (signal == S_SIGNALED) {
        process->status = EXITED_P;
        log_events(SIGNALED, global_ticks, process->pid, process->priority, process->process);
        process->status = ZOMBIED_P;
        log_events(ZOMBIE, global_ticks, process->pid, process->priority, process->process);
        remove_from_scheduler(process);
        orphan_check(process); 

        if (process == active_process) {
            k_unblock(process->parent);
        }
        return 0;
    }

    return 0;
}

void k_process_cleanup(pcb_t *process)
{
    if (process->parent) {
        children_list *child = process->parent->children;
        children_list *prev = NULL;

        while (child) {
            if (child->pid == process->pid) {
                if (prev) {
                    prev->next = child->next;
                    break;
                } else {
                    process->parent->children = child->next;
                    break;
                }
            }

            prev = child;
            child = child->next;
        }
    }

    free_pcb(process);
    return;
}
