#ifndef KERNEL_HEADER
#define KERNEL_HEADER

#include "scheduler.h"
#include "PCB.h"

void idle_process();

void make_context(ucontext_t *ucp,  void (*func)(), char *argv[]);

// kernel level
void k_block(pcb_t *parent);

void k_unblock(pcb_t *parent);

pcb_t *k_process_create(pcb_t *parent, bool is_shell);

int k_process_kill(pcb_t *process, int signal);

void k_process_cleanup(pcb_t *process);

#endif