#include "stdio.h"
#include "string.h"

#include "user.h"
#include "kernel.h"
#include "logger.h"

extern node *active_node;
extern ucontext_t idle_context;
extern int ticks;

pid_t p_spawn(void (*func)(), char **argv, int fd0, int fd1) {
    bool is_shell = false;

    // printf("%s\n", argv[0][1]);

    if (strcmp(argv[0], "shell") == 0) {
        is_shell = true;
    }

    pcb_t *parent = is_shell ? NULL : (pcb_t *)active_node->payload;

    pcb_t *child = k_process_create(parent, is_shell);

    child->fd0 = fd0;
    child->fd1 = fd1;

    child->process = malloc(sizeof(char) * (strlen(argv[0]) + 1));
    strcpy(child->process, argv[0]);

    make_context(&(child->context), func, argv);

    log_events(CREATE, ticks, child->pid, child->priority, child->process);

    node *n = init_node(child);
    add_to_scheduler(n);
    return child->pid;
}

void p_sleep(unsigned int ticks) {
    pcb_t *active_process = (pcb_t *)active_node->payload;
    active_process->ticks = ticks;
    printf("ticks = %d\n", ticks);
    node *parent = search_in_scheduler(active_process->parent->pid);
    k_block(parent);
    swapcontext(&active_process->context, &idle_context);
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