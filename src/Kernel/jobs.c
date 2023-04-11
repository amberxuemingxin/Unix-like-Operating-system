#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jobs.h"
#include "PCB.h"

const char STATUS[3][9] = {"Running", "Stopped", "Finished"};

// flatten the cmds into one-line command
char *flatten(struct parsed_command *cmd)
{
    // calculate the size
    int size = 0;
    for (size_t i = 0; i < cmd->num_commands; i++)
    {
        for (char **arguments = cmd->commands[i]; *arguments != NULL; ++arguments)
        {
            size += strlen(*arguments);
            size++; // add space after each cmd
        }

        if (i == 0 && cmd->stdin_file != NULL)
        {
            size += 2; // < + space
            size += strlen(cmd->stdin_file);
            size++;
        }

        if (i == cmd->num_commands - 1)
        {
            if (cmd->stdout_file != NULL)
            {
                if (cmd->is_file_append)
                {
                    size += 3; // >> + space
                    size += strlen(cmd->stdout_file);
                    size++;
                }
                else
                {
                    size += 2; // > + space
                    size += strlen(cmd->stdout_file);
                    size++;
                }
            }
        }
        else
        {
            size += 2;
        }
    }
    if (cmd->is_background)
    {
        size++; // &
    }
    size++; // EOF

    char *string = malloc(size * sizeof(char));
    string[0] = '\0';

    for (size_t i = 0; i < cmd->num_commands; ++i)
    {
        for (char **arguments = cmd->commands[i]; *arguments != NULL; ++arguments)
        {
            strcat(string, *arguments);
            strcat(string, " ");
        }

        if (i == 0 && cmd->stdin_file != NULL)
        {
            strcat(string, "< ");
            strcat(string, cmd->stdin_file);
            strcat(string, " ");
        }

        if (i == cmd->num_commands - 1)
        {
            if (cmd->stdout_file != NULL)
            {
                if (cmd->is_file_append)
                {
                    strcat(string, ">> ");
                }
                else
                {
                    strcat(string, "> ");
                }
                strcat(string, cmd->stdout_file);
                strcat(string, " ");
            }
        }
        else
        {
            strcat(string, "| ");
        }
    }

    return string;
}

job *init_job(struct parsed_command *cmd, job_list *list) {
    job *j = malloc(sizeof(job));
    j->cmd = flatten(cmd);
    j->background = cmd->is_background;
    j->status = RUNNING_P;
    j->next = NULL;

    if (cmd->is_background) {
        j->jid = list->max_jid + 1;
        list->max_jid++;
    } else {
        j->jid = 0;
    }

    return j;
}

job_list *init_job_list() {
    job_list *list = malloc(sizeof(job_list));
    list->max_jid = 0;
    list->queue_running = NULL;
    list->queue_stopped = NULL;

    return list;
}

void free_job(job *j) {
    free(j->cmd);
    free(j);
}

void free_all_jobs(job_list *list) {
    job *tmp;
    job *cur = list->queue_running;
    while (cur) {
        tmp = cur;
        cur = cur->next;
        free_job(tmp);
    }

    cur = list->queue_stopped;
    while (cur) {
        tmp = cur;
        cur = cur->next;
        free_job(tmp);
    }

    free(list);
}

job *find_by_jid(int jid, job_list *list) {
    job *tmp = list->queue_running;
    while (tmp)
    {
        if (tmp->jid == jid)
        {
            return tmp;
        }
        tmp = tmp->next;
    }

    tmp = list->queue_stopped;
    while (tmp != NULL)
    {
        if (tmp->jid == jid)
        {
            return tmp;
        }
        tmp = tmp->next;
    }

    // return null if not found
    return NULL;
}

void print_all_jobs(job_list *list) {
    for (int i = 1; i <= list->max_jid; i++)
    {
        job *curr = find_by_jid(i, list);
        if (curr != NULL)
        {
            fprintf(stderr, "[%d] %s (%s)\n", curr->jid, curr->cmd, STATUS[curr->status]);
        }
    }
}

/* remove a job from list
* if stopped = true, remove from the stopped Q
* else, remove from the curr Q
*/ 
void remove_job(job *j, job_list *list, bool stopped)
{
    job *prev = NULL;
    job *tmp;
    if (stopped) {
        tmp = list->queue_stopped;
    } else {
        tmp = list->queue_running;
    }

    while (tmp) {
        if (tmp == j) {
            if (prev) {
                prev->next = j->next;
            } else {
                if (stopped) {
                    list->queue_stopped = j->next;
                } else {
                    list->queue_running = j->next;
                }
            }
            j->next = NULL;
            return;
        }
        prev = tmp;
        tmp = tmp->next;
    }
}

/* add a job into the head of the Q
* if stopped, add it as the head of stopped Q
* if !stopped, add it as the head of curr Q
*/
void add_to_head(job *j, job_list *list, bool stopped) {
    job *prev;

    if (stopped) {
        prev = list->queue_stopped;
        list->queue_stopped = j;
    } else {
        prev = list->queue_running;
        list->queue_running = j;
    }

    j->next = prev;
}
