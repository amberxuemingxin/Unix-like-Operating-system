#ifndef KERNEL_HEADER
#define KERNEL_HEADER

#include "scheduler.h"
#include "PCB.h"

void make_context(ucontext_t *ucp,  void (*func)(), char *argv[]);

// kernel level
pcb_t *k_shell_create();

void k_process_create(pcb_t *parent);

int k_process_kill(pcb_t *process, int signal);

void k_process_cleanup(pcb_t *process);

#endif