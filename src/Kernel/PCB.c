#include "PCB.h"
#include <stdio.h> // debug purpose

void free_pcb(pcb_t *p) {
    pcb_t *tmp;
    pcb_t *child_head = p->children;
    while (child_head) {
        tmp = child_head;
        child_head = child_head->next;
        free_pcb(tmp);
    }

    // free(p->process);
    // free(p->context.uc_stack.ss_sp);
    free(p);
}