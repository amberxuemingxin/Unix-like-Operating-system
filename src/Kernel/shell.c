#include <signal.h>

#include "shell.h"
#include "parser.h"
#include "user.h"
#include "execute.h"
#include "jobs.h"
#include "handler.h"
#include "../pennFAT/pennfatlib.h" /* f_write */
#include "../pennFAT/macro.h" /* PENNOS_STDOUT */
#include "../pennFAT/FAT.h"

job_list *list;
bool interactive;

void sigint_handler(int signo)
{
    if (list->fg_job)
    {
        p_kill(list->fg_job->pid, S_SIGNALED);
    }
}

void sigtstp_handler(int signo)
{
    job *j = list->fg_job;
    if (j && j->pid != 1)
    {
        p_kill(j->pid, S_SIGSTOP);
        // change the status of the job to STOPPED
        j->status = STOPPED_P;

        // remove the job from the previous list
        remove_job(j, list, false);
        // and add to the stopped list
        add_to_head(j, list, true);

        list->fg_job = NULL;

        printf("Stopped: %s\n", j->cmd);
    }
    
}

void shell_loop()
{
    list = init_job_list();
    interactive = isatty(STDIN_FILENO);

    // ctrl+C behavior
    if (signal(SIGINT, sigint_handler) == SIG_ERR)
    {
        perror("Fail to catch SIGINT!\n");
        exit(EXIT_FAILURE);
    }

    // ctrl+Z behavior
    if (signal(SIGTSTP, sigtstp_handler) == SIG_ERR)
    {
        perror("Fail to catch SIGTSTP!\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        /* wait for bg jobs */
        job *j = list->queue_running;

        // iterate through all running processes
        while (j)
        {
            job *tmp = j->next;
            if (j->background)
            {
                // if it's a background process, wait on it
                int return_value = p_waitpid(j->pid, &j->status, true);
                // if it finished running
                if (return_value == j->pid)
                {
                    // remove the job from the current list and free job
                    remove_job(j, list, false);
                    printf("Finished: %s\n", j->cmd);
                    free_job(j);
                }
            }
            j = tmp;
        }

        // first prompt to the user
        if (interactive)
        {
            // int return_value = write(STDERR_FILENO, "$ ", strlen("$ "));
            int return_value = f_write(PENNOS_STDOUT, "$ ", 3);
            if (return_value == -1)
            {
                perror("Fail to write!\n");
                exit(EXIT_FAILURE);
            }
        }

        // get user input
        char *user_input = NULL;
        size_t size = 0;
        ssize_t chars_read;

        chars_read = getline(&user_input, &size, stdin);
        if (chars_read <= 0)
        {
            free(user_input);
            continue;
        }
        // parse user input
        struct parsed_command *cmd;
        int flag = parse_command(user_input, &cmd);

        free(user_input);

        if (flag < 0)
        {
            perror("invalid");
            continue;
        }
        if (flag > 0)
        {
            perror("invalid");
            continue;
        }

        if (cmd->num_commands >= 1)
        {
            // send to the command handler
            cmd_handler(cmd);
        }

        free(cmd);
    }
}