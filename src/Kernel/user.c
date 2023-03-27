#include "stdio.h"
#include "string.h"

#include "user.h"
#include "kernel.h"
#include "logger.h"

extern node *active_node;
extern queue *queue_sleep;
extern int ticks;

pid_t p_spawn(void (*func)(), char *argv[], int fd0, int fd1) {
    pcb_t *parent = (pcb_t *)active_node;
    pcb_t *child = k_process_create(parent);

    child->fd0 = fd0;
    child->fd1 = fd1;

    child->process = malloc(sizeof(char) * (strlen(argv[0]) + 1));
    strcpy(child->process, argv[0]);

    make_context(&(child->context), func, argv);

    log_events(CREATE, ticks, child->pid, child->priority, child->process);

    return child->pid;
}

void p_sleep(int sleep_ticks) {
    pcb_t *active_process = (pcb_t *)active_node->payload;

    active_process->status = BLOCKED;
    active_process->ticks = sleep_ticks;
    add_node(queue_sleep, active_node);
}

void p_exit() {
    if (active_node == NULL) {
        perror("no active_process\n");
        return;
    }

    /* current process is shell */
    pcb_t *active_process = (pcb_t *)active_node->payload;
    if (active_process->pid == 1) {
        exit(EXIT_SUCCESS);
    }

    /* current process is not shell */
    active_process->status = EXITED_P;
    wait_for_processes(active_node);
}