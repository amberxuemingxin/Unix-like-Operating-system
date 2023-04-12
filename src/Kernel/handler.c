#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "handler.h"
#include "jobs.h"
#include "execute.h"
#include "user.h"
#include "kernel.h"

extern job_list *list;

void cmd_handler(struct parsed_command *cmd) {
    if (strcmp(cmd->commands[0][0], "jobs") == 0)
    {
        print_all_jobs(list);
        return;
    }
    else if (strcmp(cmd->commands[0][0], "fg") == 0)
    {
        job *job = NULL;

        if (cmd->commands[0][1])
        { // by jid
            int jid = atoi(cmd->commands[0][1]);

            if (jid != 0)
            {
                job = find_by_jid(jid, list);
            }
        }
        else
        { // default: prioritize with stopped
            if (list->queue_stopped != NULL)
            {
                job = list->queue_stopped;
            }
            else
            {
                job = list->queue_running;
            }
        }

        // throw an error if job doesn't exist
        if (job == NULL)
        {
            fprintf(stderr, "Job not exist!\n");
            return;
        }

        // bring job to fg
        list->fg_job = job;
        job->background = false;
        // TODO
        // tcsetpgrp(STDIN_FILENO, job->pgid);

        // if the job is stopped, resume it
        if (job->status == STOPPED_P)
        {
            // TODO resume_job(job);
            fprintf(stderr, "Restarting: %s\n", job->cmd);
        }
        else
        {
            fprintf(stderr, "%s\n", job->cmd);
        }

        // TODO wait_for_fg(job);

        return;
    }
    else if (strcmp(cmd->commands[0][0], "bg") == 0)
    {
        job *job = NULL;

        if (cmd->commands[0][1])
        { // by jid

            int jid = atoi(cmd->commands[0][1]);

            if (jid != 0)
            {
                job = find_by_jid(jid, list);
            }
        }
        else
        { // default
            job = list->queue_stopped;
        }

        // throw an error if job doesn't exist
        if (job == NULL)
        {
            fprintf(stderr, "Job not exist!\n");
            return;
        }

        if (job->status != STOPPED_P)
        {
            fprintf(stderr, "Job is running already!\n");
            return;
        }

        // TODO resume_job(job);
        fprintf(stderr, "Running: %s\n", job->cmd);

        return;
        
    } else if (strcmp(cmd->commands[0][0], "nice") == 0) {
        // TODO
    } else if (strcmp(cmd->commands[0][0], "nice_pid") == 0) {
        char *priority_string = cmd->commands[0][1];
        char *pid_string = cmd->commands[0][2];
        int priority;
        int pid;

        if (strcmp(priority_string, "-1") == 0) {
            priority = -1;
        } else if (strcmp(priority_string, "0") == 0) {
            priority = 0;
        } else if (strcmp(priority_string, "1") == 0) {
            priority = 1;
        } else {
            fprintf(stderr, "Please enter the correct priority value\n");
            return;
        }

        pid = atoi(pid_string);
        if (pid == 0) {
            fprintf(stderr, "Please enter the correct pid value\n");
        }

        p_nice(pid, priority);
    } else if (strcmp(cmd->commands[0][0], "logout") == 0) {
        free_all_jobs(list);
        free(cmd);
        p_exit();
    }

    // execute the cmd
    if (cmd->num_commands >= 1)
    {
        // init job here
        job *job = init_job(cmd, list);
        int return_value = execute(cmd, job);

        /* if execution success */
        if (return_value == 0) {
            /* block the parent if it's a fg job */
            if (!job->background) {
                list->fg_job = job;
                if (p_waitpid(job->pid, &job->status, false) == job->pid) {
                    pcb_t *process = search_in_scheduler(job->pid);

                    remove_job(job, list, false);
                    free_job(job);
                    list->fg_job = NULL;

                    k_process_cleanup(process);
                }
            } else { /* else, won't block the parent */
                printf("Running: %s\n", job->cmd);
            }
        }
    }
}
