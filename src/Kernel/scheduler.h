/**
 * @file scheduler.h
 * @brief A scheduler schedules the next process every 100 miliseconds and keeps track with all processes' livecycles by queues.
 * 
 */

#ifndef SCHEDULER_HEADER
#define SCHEDULER_HEADER

#include "queue.h"

/**
 * @{ \name Time Quantum Constant
 */
#define CENTISECOND 10000 /**< which is 100 millisconds */
/**
 * @}
 */
/**
 * @{ \name Flag for Return Values
 */
#define SUCCESS 0
#define FAILURE -1
/**
 * @}
 */

/**
 * @brief Initialize the scheduler, which has 3 ready queues, 1 block queue, and 1 zombie queue.
*/
void init_scheduler();

/**
 * @brief The scheduler's timer. 
*/
void set_timer();

/**
 * @brief Alarm handler that call the scheduler every 100 miliseconds.
*/
void set_alarm_handler();

/**
 * @brief Search in the scheduler by the given pid.
 * It will only search in the "living" processes. Processes in ready queues and block queue are considered living.
 * 
 * @param pid The pid to search for.
 * @return If the process is still living in the scheduler, return the process; else, return NULL.
*/
pcb_t *search_in_scheduler(pid_t pid);

/**
 * @brief Search in the scheduler by the given pid.
 * It will only search in the "dead" processes. Processes in zombie queue are considered dead.
 * 
 * @param pid The pid to search for.
 * @return If the process is dead and exists in the scheduler, return the process; else, return NULL.
*/
pcb_t *search_in_zombies(pid_t pid);

/**
 * @brief Schedule the next process every 100 miliseconds.
 * 
 * The scheduler will decrement ticks for all sleeping processes, pick the next process by generating weighted random number, and set the context to the picked process.
 * If no process is in the scheduler, the scheduler will schedule the idle process.
*/
void schedule();

/**
 * @brief Add a process into the scheduler's ready queues based on the process's nice value.
 * 
 * @param p The process to add.
*/
void add_to_scheduler(pcb_t *p);

/**
 * @brief Remove a process from the scheduler's ready queues.
 * 
 * @param p The process to remove.
 * @return 0 if success, -1 if the process is not in the ready queues.
*/
int remove_from_scheduler(pcb_t *p);

/**
 * @brief Move a process from the ready queue to the block queue.
 * 
 * @param p The process to move.
*/
void ready_to_block(pcb_t *p);

/**
 * @brief Move a process from the block queue to the ready queue.
 * 
 * @param p The process to move.
*/
void block_to_ready(pcb_t *process);

/**
 * @brief Exit the scheduler gracefully.
*/
void exit_scheduler();

/**
 * @brief Print all living processes and zombies.
 * 
 * Used by shell built-ins. 
*/
void print_all_process();

 #endif