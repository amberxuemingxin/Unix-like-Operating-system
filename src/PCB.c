#include "PCB.h"

pcb_t *pcb_spawn(ucontext_t *context, int pid, int ppid, int priority, struct parsed_command *cmd) {

    pcb_t *p = (pcb_t *)malloc(sizeof(pcb_t));
    p->pid = pid;
    p->ppid = ppid;
    p->priority = priority;
    p->fd0 = cmd->stdin_file;
    p->fd1 = cmd->stdout_file;
    p->status = RUNNING;
    p->context = context;
    return p;
}

void free_pcb(pcb_t *p) {
    free(p);
}