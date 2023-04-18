#include "PCB.h"

// free the pcb, yay!
void free_pcb(pcb_t *p)
{
    free(p->children);

    free(p);
}