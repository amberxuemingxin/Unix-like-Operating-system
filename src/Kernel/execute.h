/**
 * @file execute.h
 * @brief execute whatever commands that are passed in
 */

#include "parser.h"
#include "jobs.h"

/**
 * @brief The execute function that executes whatever commands that are passed in
 * and attach to the job list
 * @param cmd the parsed command
 * @param job the job added
 * @param priority the priority of the job
 * @return an int that indicates whether it is executed correctly
 *
 */
int execute(struct parsed_command *cmd, job *job, int priority);