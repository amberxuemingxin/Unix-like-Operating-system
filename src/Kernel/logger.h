/**
 * @file logger.h
 * @ brief logger structure that logs the behavior of all processes
 */

#include <stdio.h>
#include <stdlib.h>

/* Macros for the logger */
#define CREATE 0
#define SIGNALED 1
#define EXITED 2
#define ZOMBIE 3
#define ORPHAN 4
#define WAITED 5
#define SCHEDULE 6
#define BLOCKED 7
#define UNBLOCKED 8
#define STOPPED 9
#define CONTINUED 10

/**
 * @brief Generate the current time stamp and format it as a char array to print out to the log file
 *
 * @return return the char array of time stamp generated
 */
char *time_stamp();

/**
 * @brief log the current event that just happened and output to the log file
 * @param type type of the event
 * @param ticks the number of ticks the event took place
 * @param pid the process id of the event
 * @param priority the priority of the process
 * @param process the name of the process
 */
void log_events(int type, int ticks, int pid, int priority, char *process);

/**
 * @brief log the nice event and output to the log file
 * @param ticks the number of ticks the nice event took place
 * @param old_priority the original priority before the NICE call
 * @param new_priority the new priority assigned
 * @param process the name of the process
 */
void log_nice(int ticks, int pid, int old_priority, int new_priority, char *process);

/**
 * @brief free the logger data structure
 */
void free_logger();