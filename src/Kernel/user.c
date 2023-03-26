#include "user.h"

extern node *active_node;

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

}