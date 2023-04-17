/**
 * @file queue.h
 * @brief A queue is a utility structure that define a single linked list used by the scheduler.
 */

#ifndef QUEUE_HEADER
#define QUEUE_HEADER

#include <stdlib.h>
#include "PCB.h"

/**
 * @brief A single linked list for processes. 
*/
typedef struct LinkedList {
    pcb_t *head; /**< A pointer to the first element in this queue. It will be NULL when the queue is empty. */
    int length; /**< The length of the queue. */
} queue;

/**
 * @brief Initialize an empty queue.
 * 
 * @return A pointer to the initialized queue.
*/
queue *init_queue();

/**
 * @brief Add a process to the back of the queue.
 * 
 * @param q The destination queue.
 * @param p The process to add.
*/
void add_process(queue *q, pcb_t *p);

/**
 * @brief Remove a process from the queue.
 * 
 * @param q The destination queue.
 * @param p The process to remove.
 * @return If the process is not found, return NULL; else, return the process that's been removed.
*/
pcb_t *remove_process(queue *q, pcb_t *p);

#endif