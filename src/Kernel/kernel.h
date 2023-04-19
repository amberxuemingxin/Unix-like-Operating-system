/**
 * @file kernel.h
 * @brief A kernel provides all kenel level functions that the OS would need.
 */

#ifndef KERNEL_HEADER
#define KERNEL_HEADER

#include <signal.h>            // sigaction, sigemptyset, sigfillset, signal
#include "header.h"
#include "scheduler.h"
#include "PCB.h"

/**
 * @brief An idle process that would be scheduled when no process is ready.
*/
void idle_process();

/**
 * @brief A process of sending the terminate signal. 
 * 
 * All normal processes would be linked to it to send the signal to the caller process.
*/
void exit_process();

/**
 * @brief Sets all necessary information for makecontext.
 * 
 * @param ucp The context.
 * @param func The function.
 * @param argc The number of arguments.
 * @param argv The list of arguments.
*/
void make_context(ucontext_t *ucp, void (*func)(), int argc, char *argv[]);

/**
 * @brief Block a process.
 * 
 * @param parent The process to be blocked.
*/
void k_block(pcb_t *parent);

/**
 * @brief Unblock a process.
 * 
 * @param parent The process to be unblocked.
*/
void k_unblock(pcb_t *parent);

/**
 * @brief Create a process at the kernel level.
 * 
 * @param parent The caller process. Nullable.
 * @param is_shell The flag indicates if the created process is shell.
 * @return The process created.
*/
pcb_t *k_process_create(pcb_t *parent, bool is_shell);

/**
 * @brief Send the signal to the specified process.
 * 
 * @param process The destination process.
 * @param signal The signal to send.
 * @return 0 if success, -1 if fail.
*/
int k_process_kill(pcb_t *process, int signal);

/**
 * @brief Clean up a process.
 * 
 * @param process The process to clean up.
*/
void k_process_cleanup(pcb_t *process);

#endif