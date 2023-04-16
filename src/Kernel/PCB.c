#include "PCB.h"

void free_pcb(pcb_t *p) {
    free(p->children);

    free(p);
}