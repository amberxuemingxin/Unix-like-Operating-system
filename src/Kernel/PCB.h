/**
 * @file PCB.h
 * @brief A PCB is a structure that describes all the needed information about a running process.
 */

#ifndef PCB_HEADER
#define PCB_HEADER

#include <sys/types.h>
#include <ucontext.h> 
#include <stdlib.h>
#include "parser.h"

/**
 * @{ \name Possible Status for PCB
 */
#define RUNNING_P 0 /**< The process is running normally. It can be picked by the scheduler. */
#define STOPPED_P 1 /**< The process is stopped by signal. It can be resumed. */
#define EXITED_P 2 /**< The process is finished or terminated. It can't be resumed. */
#define ZOMBIED_P 3 /**< The process is finished or terminated while the parent it's not waiting for it. It can't be resumed. */
#define BLOCKED_P 4 /**< The process is blocked by other processes. It can't be picked by the scheduler until it's unblocked. */
/**
 * @}
 */

/**
 * @brief A single linked list for all children's pid of a process.
 * 
 * This structure will be used by the struct pcb_t. 
*/
typedef struct children_def {
    pid_t pid;                      /**< the pid of current child. */
    struct children_def *next;      /**< the next child. */
} children_list;

/**
 * @brief A structure keeping all necessary information for a process to run.
 * 
*/
typedef struct pcb_def
{
    char *process; /**< The name of the process. Notice that any arguments will be ignored. */

    int fd0; /**< File descriptor for stdin. */
    int fd1; /**< File descriptor for stdout. */

    pid_t pid; /**< The process ID of itself. */
    pid_t ppid; /**< The process ID of its parent. */
    pid_t pgid; /**< The group process ID of itself. */

    struct pcb_def *parent; /**< A pointer pointing to its parent. A process only has one parent. Notice that the shell has no parent. */
    struct pcb_def *next; /**< A pointer pointing to the next process in the scheduler. */

    int status; /**< The status of a process: RUNNING_P, STOPPED_P, FINISHED_P, ZOMBIED_P, and BLOCKED_P. */
    int priority; /**< The nice value of itself. Can be -1 (high), 0(mid), 1(low). */
    int ticks; /**< The number of ticks left for the process to awake. Only the sleep process will use this field. */
    int num_blocks; /**< The number of processes that is blocking it. */

    children_list *children; /**< The linked list of children's pid for children management. */
    bool background; /**< If the process is a background process. */
    ucontext_t context; /**< The execution context of itself. */
} pcb_t;

/**
 * @brief A helper method to free the PCB struct.
 * 
 * @param p The process to free.
*/

void free_pcb(pcb_t *p);

#endif
