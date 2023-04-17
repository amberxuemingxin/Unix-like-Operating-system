#include "user.h"
#include "kernel.h"
#include "logger.h"

extern pcb_t *active_process;
extern ucontext_t idle_context;
extern ucontext_t scheduler_context;
extern queue *queue_zombie;
extern int global_ticks;

/* fork a new process
* return = the pid of the new process
*/
pid_t p_spawn(void (*func)(), char *argv[], int num_arg, int fd0, int fd1, int priority, bool background) {
    bool is_shell = false;

    if (strcmp(argv[0], "shell") == 0) {
        is_shell = true;
    }

    pcb_t *parent = is_shell ? NULL : active_process;

    pcb_t *child = k_process_create(parent, is_shell);

    child->fd0 = fd0;
    child->fd1 = fd1;
    child->priority = priority;
    child->background = background;

    child->process = malloc(sizeof(char) * (strlen(argv[0]) + 1));
    strcpy(child->process, argv[0]);
    
    make_context(&(child->context), func, num_arg, &argv[1]);

    log_events(CREATE, global_ticks, child->pid, child->priority, child->process);

    add_to_scheduler(child);
    return child->pid;
}

/* suspend the process for a specific amount of seconds
*/
void p_sleep(unsigned int ticks) {
    pcb_t *sleep_process = active_process;
    sleep_process->ticks = ticks;
    // pcb_t *parent = sleep_process->parent;
    /* block itself */
    // printf("sleep block %s in p_sleep\n", active_process->process);
    k_block(active_process);
}

/* send a signal to a process group
* - pid > 0, send the signal to the specified pid
* - pid = 0, send the signal to all processes that the pgid = the active process's pgid (sender)
* return:
* - 0 -> success
* - -1 -> failure
*/
int p_kill(pid_t pid, int sig) {
    /* If pid is equal to 0, 
    kill() sends its signal to all processes whose process group ID is equal to that of the sender. */
    if (pid == 0) {
        k_process_kill(active_process, sig);
        return SUCCESS;

    } else {
        pcb_t *p = search_in_scheduler(pid);

        if (p) {
            k_process_kill(p, sig);
            return SUCCESS;
        } else { /* pid not found */
//            printf("Pid not found\n");
            return FAILURE;
        }
    }
}

/* suspend the caller until a child ends/stops
* - pid > 0, wait for the specified pid
* - pid = -1, wait for any child of the caller
* - nohang = true, return immediately
* - nohang = false, block the caller until any children change the state
*/
pid_t p_waitpid(pid_t pid, int *wstatus, bool nohang) {
        
    if (pid == -1) { /* wait for any children processes */
        children_list *child = active_process->children;
        
        if (child == NULL) {
            /* when nohang exit */
            return -1;
        }

        /* global as the caller */
        if (!nohang) {
            // printf("%d block %s in waitpid\n", active_process->children->pid, active_process->process);
            k_block(active_process);
        }

        while (child) {
            pcb_t *p = search_in_scheduler(child->pid) ? search_in_scheduler(child->pid) : search_in_zombies(child->pid);
            if (p == NULL) {
                return 0;
            }
            if (W_WIFEXITED(p->status)) {
                pid_t return_value = p->pid;
                log_events(WAITED, global_ticks, p->pid, p->priority, p->process);
                if (remove_from_scheduler(p) == FAILURE) {
                    remove_process(queue_zombie, p);
                }
                k_process_cleanup(p);
                return return_value;
            }
            child = child->next;
        }

    } else { /* wait for specific process */
        pcb_t *p = search_in_scheduler(pid) ? search_in_scheduler(pid) : search_in_zombies(pid);

        if (p == NULL) {
            printf("Waitpid can't find the pid %d\n", pid);
            return 0;
        }

        /* global as the caller */
        if (!nohang) {
            k_block(active_process);
        }

        if (W_WIFEXITED(p->status)) {
            log_events(WAITED, global_ticks, p->pid, p->priority, p->process);
            if (remove_from_scheduler(p) == FAILURE) {
                remove_process(queue_zombie, p);
            }
            k_process_cleanup(p);
            return pid;
        } else {
            return 0;
        }

    }
    
    return 0;
}

/* end the caller process
*/
void p_exit() {
    pid_t cur_pid = active_process->pid;
    p_kill(cur_pid, S_SIGTERM);
    k_process_cleanup(active_process);

    // check zombie & orphan everytime after p_kill
    if (cur_pid == 1) {
        free(idle_context.uc_stack.ss_sp);
        free(scheduler_context.uc_stack.ss_sp);

        exit_scheduler();
        free_logger();

        exit(EXIT_SUCCESS);
    }
}

int p_nice(pid_t pid, int priority) {
    pcb_t *p = search_in_scheduler(pid);

    if (p) {
        if (p->priority != priority) {
            int old_priority = p->priority;

            remove_from_scheduler(p);
            p->priority = priority;
            add_to_scheduler(p);

            log_nice(global_ticks, pid, old_priority, priority, p->process);
            return SUCCESS;
        }
    }

    return FAILURE;
}