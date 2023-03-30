#include "PCB.h"

void free_pcb(pcb_t *p) {
    free(p->process);
    free(p->context.uc_stack.ss_sp);

    pcb_t *tmp;
    pcb_t *child_head = p->children;
    while (child_head) {
        tmp = child_head;
        child_head = child_head->next;
        free_pcb(tmp);
    }

    pcb_t *zombie_head = p->zombies;
    while (zombie_head)
    {
        tmp = zombie_head;
        zombie_head = zombie_head->next;
        free_pcb(tmp);
    }

    free(p);
}