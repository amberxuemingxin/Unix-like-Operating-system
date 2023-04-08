#include <stdio.h>
#include <string.h>

#include "user.h"
#include "kernel.h"
#include "logger.h"
#include "scheduler.h"

extern pcb_t *active_process;
extern ucontext_t idle_context;
extern ucontext_t scheduler_context;
extern int ticks;

pid_t p_spawn(void (*func)(), char *argv[], int num_arg, int fd0, int fd1) {
    bool is_shell = false;

    if (strcmp(argv[0], "shell") == 0) {
        is_shell = true;
    }

    pcb_t *parent = is_shell ? NULL : active_process;

    pcb_t *child = k_process_create(parent, is_shell);

    child->fd0 = fd0;
    child->fd1 = fd1;

    child->process = malloc(sizeof(char) * (strlen(argv[0]) + 1));
    strcpy(child->process, argv[0]);

// tmp {"sleep", "1", NULL}
    
    make_context(&(child->context), func, num_arg, &argv[1]);

    log_events(CREATE, ticks, child->pid, child->priority, child->process);

    add_to_scheduler(child);
    return child->pid;
}

void p_sleep(unsigned int ticks) {
    active_process->ticks = ticks;
    pcb_t *parent = active_process->parent;
    k_block(parent);
    swapcontext(&active_process->context, &idle_context);
}

int p_kill(pid_t pid, int sig) {
    /* If pid is equal to 0, 
    kill() sends its signal to all processes whose process group ID is equal to that of the sender. */
    if (pid == 0) {
        k_process_kill(active_process, sig);
        return 0;

    } else {
        pcb_t *p = search_in_scheduler(pid);

        if (p) {
            k_process_kill(p, sig);
            return 0;
        } else { /* pid not found */
            return -1;
        }
    }
}

pid_t p_waitpid(pid_t pid, int *wstatus, bool nohang) {
    /* global as the caller */
    if (nohang) { /* return the result immediately */
        if (pid == -1) { /* wait for any children processes */
            pcb_t *child = active_process->children;
            
            if (child == NULL) {
                return 0;
            }

            while (child) {
                if (W_WIFEXITED(child->status)) {
                    return child->pid;
                }
                child = child->next;
            }

            return 0;
        } else { /* wait for specific process */
            pcb_t *p = search_in_scheduler(pid);

            if (p == NULL) {
                return 0;
            }
            if (W_WIFEXITED(p->status)) {
                return pid;
            } else {
                return 0;
            }
        }
    } else {

        if (pid == -1) { /* wait for any children processes */
            pcb_t *child = active_process->children;

            if (child == NULL) {
                return 0;
            }

            while (child) {
                if (W_WIFEXITED(child->status)) {
                    return child->pid;
                }
                child = child->next;
            }

            /* no child is finished */
            /* block the caller */
            k_block(active_process);
            /* block queue:  */

        } else { /* wait for specific process */

            pcb_t *p = search_in_scheduler(pid);

            if (p == NULL) {
                return 0;
            }
            if (W_WIFEXITED(p->status)) {
                return pid;
            } else {
                return 0;
            }

            k_block(active_process);
        }
    }
    return 0;
}

void p_exit() {
    pcb_t *shell_process = search_in_scheduler(1);
    p_kill(1, S_SIGTERM);
    k_process_cleanup(shell_process);

    free(idle_context.uc_stack.ss_sp);
    free(scheduler_context.uc_stack.ss_sp);

    exit_scheduler();
    free_logger();

    exit(EXIT_SUCCESS);
}

int p_nice(pid_t pid, int priority) {
    pcb_t *p = search_in_scheduler(pid);

    if (p) {
        if (p->priority != priority) {
            int old_priority = p->priority;

            remove_from_scheduler(p);
            p->priority = priority;
            add_to_scheduler(p);

            log_nice(ticks, pid, old_priority, priority, p->process);
            return 0;
        }
    }

    return 1;
}