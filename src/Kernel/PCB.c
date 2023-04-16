#include "PCB.h"
#include <stdio.h> // debug purpose

void free_pcb(pcb_t *p) {
    free(p->children);

    // free(p->process);
    // free(p->context.uc_stack.ss_sp);
    free(p);
}