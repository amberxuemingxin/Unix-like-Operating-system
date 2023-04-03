#include "stdio.h"
#include "string.h"

#include "user.h"
#include "kernel.h"
#include "logger.h"

extern pcb_t *active_process;
extern ucontext_t idle_context;
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

// tmp {"sleep", &num, NULL}
    
    make_context(&(child->context), func, num_arg, &argv[1]);

    log_events(CREATE, ticks, child->pid, child->priority, child->process);

    add_to_scheduler(child);
    return child->pid;
}

void p_sleep(unsigned int ticks) {
    active_process->ticks = ticks;
    printf("ticks = %d\n", ticks);
    pcb_t *parent = active_process->parent;
    k_block(parent);
    swapcontext(&active_process->context, &idle_context);
}

void p_exit() {
    if (active_process == NULL) {
        perror("no active_process\n");
        return;
    }

    /* current process is shell */
    if (active_process->pid == 1) {
        exit(EXIT_SUCCESS);
    }

    /* current process is not shell */
    active_process->status = EXITED_P;
    wait_for_processes(active_process);
}