/**
 * @file jobs.h
 * @brief the jobs data structure, which manages a linked list of jobs, and relevant functions.
 */

#ifndef JOB_HEADER
#define JOB_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "parser.h"

/**
 * @brief the jobs data structure, essentially a linked list node
 * which has the pid, pgid, background and other fields
 */
typedef struct jobs
{
    pid_t pid;
    pid_t pgid;

    char *cmd;
    bool background;
    int fd0;
    int fd1;

    int jid;
    int status;
    struct jobs *next;
} job;

/**
 * @brief the job queue data structure, essentially a head to the corresponding
 * linked list of running queue and stopped queue
 */
typedef struct job_queue
{
    struct jobs *queue_running;
    struct jobs *queue_stopped;
    struct jobs *fg_job;
    int max_jid;
} job_list;

/**
 * @brief initialize a job node
 * @param the parsed command and the job list the user wishes to add the job node to
 */
job *init_job(struct parsed_command *cmd, job_list *list);

/**
 * @brief initialize a job list
 */
job_list *init_job_list();

/**
 * @brief search through a designated list and find job through job id
 * @param job id the user wishes to find, and the job list the user wishes to search in
 * @return the found job, NULL if not found
 */
job *find_by_jid(int jid, job_list *list);

/**
 * @brief free the job node
 * @param the job node the user wants to free
 */
void free_job(job *j);

/**
 * @brief free all job nodes inside a job list
 * @param the job list the user wants to free
 */
void free_all_jobs(job_list *list);

/**
 * @brief print all jobs from a designated job list
 * @param the job list the user wants to print
 */
void print_all_jobs(job_list *list);

/**
 * @brief remove a job node from a designated job list
 * @param the job the user wants to remove, the job list the user wants to remove the
 * job from, and whether the job is stopped, as the job will be in a different queue
 */
void remove_job(job *j, job_list *list, bool stopped);

/**
 * @brief add a job to the head of the job list. If the job is stopped, add to the
 * stopped queue, otherwise add to the current running queue
 * @param the job the user wants to add, the job list the user wants to add to,
 * and whether the job is stopped or not
 */
void add_to_head(job *j, job_list *list, bool stopped);

/**
 * @brief add a job to the end of the job list. If the job is stopped, add to the
 * stopped queue, otherwise add to the current running queue
 * @param the job the user wants to add, the job list the user wants to add to,
 * and whether the job is stopped or not
 */
void add_to_end(job *job, job_list *list);

#endif
