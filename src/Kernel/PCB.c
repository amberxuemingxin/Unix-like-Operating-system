#include "PCB.h"
#include <stdio.h> // debug purpose

void free_pcb(pcb_t *p) {
    pcb_t *tmp;
    pcb_t *child_head = p->children;
    while (child_head) {
        printf("yay child\n");
        tmp = child_head;
        child_head = child_head->next;
        free_pcb(tmp);
    }

    pcb_t *zombie_head = p->zombies;
    while (zombie_head)
    {
        printf("yay zombie\n");
        tmp = zombie_head;
        zombie_head = zombie_head->next;
        free_pcb(tmp);
    }

    // free(p->process);
    free(p->context.uc_stack.ss_sp);
    free(p);
}