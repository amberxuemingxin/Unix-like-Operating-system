#include "handler.h"
#include "jobs.h"
#include "execute.h"
#include "user.h"
#include "queue.h"
#include "../pennFAT/macro.h"
#include "../pennFAT/pennfatlib.h"

extern job_list *list;
extern queue *queue_block;
extern pcb_t *active_process;
extern ucontext_t scheduler_context;
int priority = 0;

void my_truncate(char ***arr)
{
    (*arr) += 2; // increment the pointer by 2
    // now (*arr) points to the third element of the original array
}

void cmd_handler(struct parsed_command *cmd)
{
    // print out all jobs if the command is "jobs"
    if (strcmp(cmd->commands[0][0], "jobs") == 0)
    {
        print_all_jobs(list);
        return;
    }
    // switch the job to foreground when "fg" flag is detected
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
            f_write(PENNOS_STDOUT, "Job not exist!\n", 0);
            return;
        }

        // if the job is stopped, resume it
        if (job->status == STOPPED_P)
        {
            f_write(PENNOS_STDOUT, "Restarting: %s\n", 0, job->cmd);
            job->status = RUNNING_P;
            remove_job(job, list, true);
            add_to_head(job, list, false);
        }
        else
        {
            f_write(PENNOS_STDOUT, "%s\n", 0, job->cmd);
        }

        // bring job to fg
        p_kill(job->pid, S_SIGCONT_FG);
        list->fg_job = job;
        job->background = false;
        swapcontext(&active_process->context, &scheduler_context);

        // wait for fg
        if (p_waitpid(job->pid, &job->status, false) == job->pid)
        {
            remove_job(job, list, false);
            free_job(job);
            list->fg_job = NULL;
        }

        return;
    }
    // switch the job to background when "bg" flag is detected
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
            f_write(PENNOS_STDOUT, "Job not exist!\n", 0);
            return;
        }

        if (job->status != STOPPED_P)
        {
            f_write(PENNOS_STDOUT, "Job is running already!\n", 0);
            return;
        }

        p_kill(job->pid, S_SIGCONT_BG);
        job->status = RUNNING_P;
        remove_job(job, list, true);
        add_to_head(job, list, false);
        f_write(PENNOS_STDOUT, "Running: %s\n", 0, job->cmd);

        return;
        // give the job a nice value when "nice" flag is detected
    }
    else if (strcmp(cmd->commands[0][0], "nice") == 0)
    {
        char *priority_string = cmd->commands[0][1];

        if (strcmp(priority_string, "-1") == 0)
        {
            priority = -1;
        }
        else if (strcmp(priority_string, "0") == 0)
        {
            priority = 0;
        }
        else if (strcmp(priority_string, "1") == 0)
        {
            priority = 1;
        }
        else
        {
            f_write(PENNOS_STDOUT, "Please enter the correct priority value\n", 0);
            return;
        }

        my_truncate(cmd->commands);
    }
    // give the job a nice value when "nice" flag is detected
    else if (strcmp(cmd->commands[0][0], "nice_pid") == 0)
    {
        char *priority_string = cmd->commands[0][1];
        char *pid_string = cmd->commands[0][2];
        int priority;
        int pid;

        if (strcmp(priority_string, "-1") == 0)
        {
            priority = -1;
        }
        else if (strcmp(priority_string, "0") == 0)
        {
            priority = 0;
        }
        else if (strcmp(priority_string, "1") == 0)
        {
            priority = 1;
        }
        else
        {
            f_write(PENNOS_STDOUT, "Please enter the correct priority value\n", 0);
            return;
        }

        pid = atoi(pid_string);
        if (pid == 0)
        {
            f_write(PENNOS_STDOUT, "Please enter the correct pid value\n", 0);
        }

        p_nice(pid, priority);
        return;
    }
    // print out the user manual when the "man" flag is detected
    else if (strcmp(cmd->commands[0][0], "man") == 0)
    {
        f_write(PENNOS_STDOUT, "USER MANUAL OF PENNOS\n",0);
        f_write(PENNOS_STDOUT, "-------------------------------------------\n",0);
        f_write(PENNOS_STDOUT, "Shell Built-ins: (args with * are optional)\n",0); 
        f_write(PENNOS_STDOUT, "  cat [args]*\t\t",0);
        f_write(PENNOS_STDOUT, "reads data from the file and gives their content as output\n",0);
        f_write(PENNOS_STDOUT, "  sleep [args]\t\t",0);
        f_write(PENNOS_STDOUT, "causes the calling thread to sleep [args] seconds; args should be some integers\n",0);
        f_write(PENNOS_STDOUT, "  busy\t\t\t",0);
        f_write(PENNOS_STDOUT, "waits indefinitely\n",0);
        f_write(PENNOS_STDOUT, "  echo [args]*\t\t",0);
        f_write(PENNOS_STDOUT, "displays a line of text from [args]\n",0);
        f_write(PENNOS_STDOUT, "  ls [args]*\t\t",0);
        f_write(PENNOS_STDOUT, "lists all files in the directory\n",0);
        f_write(PENNOS_STDOUT, "  touch [args]\t\t",0);
        f_write(PENNOS_STDOUT, "creates an empty file if it does not exist, or update its timestamp otherwise\n",0);
        f_write(PENNOS_STDOUT, "  mv [arg1] [arg2]\t",0);
        f_write(PENNOS_STDOUT, "rename arg1 to arg2\n",0);
        f_write(PENNOS_STDOUT, "  cp [arg1] [arg2]\t",0);
        f_write(PENNOS_STDOUT, "copy arg1 to arg2\n",0);
        f_write(PENNOS_STDOUT, "  rm [args]\t\t",0);
        f_write(PENNOS_STDOUT, "remove [args] from the directory\n",0);
        f_write(PENNOS_STDOUT, "  chmod [args]\t\t",0);
        f_write(PENNOS_STDOUT, "changes the file mode bits of each given file according to mode\n",0);
        f_write(PENNOS_STDOUT, "  ps\t\t\t",0);
        f_write(PENNOS_STDOUT, "lists all processes on PennOS\n",0);
        f_write(PENNOS_STDOUT, "  kill [arg1] [arg2]\t",0);
        f_write(PENNOS_STDOUT, "sends a specified signal [arg1] to the specified process [arg2]\n",0);
        f_write(PENNOS_STDOUT, "-------------------------------------------\n",0);
        f_write(PENNOS_STDOUT, "Subroutines: (args with * are optional)\n",0);
        f_write(PENNOS_STDOUT, "  nice [arg1] [arg2]\t" ,0);
        f_write(PENNOS_STDOUT, "set the priority of command [arg2] as [arg1], then execute the command\n",0);
        f_write(PENNOS_STDOUT, "  nice_pid [arg1] [arg2]",0);
        f_write(PENNOS_STDOUT, "adjust the nice value for [arg2] to [arg1]\n",0);
        f_write(PENNOS_STDOUT, "  man\t\t\t",0);
        f_write(PENNOS_STDOUT, "list all available commands\n",0);
        f_write(PENNOS_STDOUT, "  bg [args]*\t\t",0);
        f_write(PENNOS_STDOUT, "continue the last stopped job, or the job specified by [args]\n",0);
        f_write(PENNOS_STDOUT, "  fg [args]*\t\t",0);
        f_write(PENNOS_STDOUT, "bring the last stopped or backgrounded job to the foreground, or the job specified by [args]\n",0);
        f_write(PENNOS_STDOUT, "  jobs\t\t\t",0);
        f_write(PENNOS_STDOUT, "list all jobs\n",0);
        f_write(PENNOS_STDOUT, "  logout\t\t",0);
        f_write(PENNOS_STDOUT, "exit the shell and shutdown PennOS\n",0);
        return;
    }
    // logout :)
    else if (strcmp(cmd->commands[0][0], "logout") == 0)
    {
        free_all_jobs(list);
        free(cmd);
        p_exit();
    }

    // execute the cmd
    if (cmd->num_commands >= 1)
    {
        // init job here
        job *job = init_job(cmd, list);
        int return_value = execute(cmd, job, priority);
        priority = 0;

        /* if execution success */
        if (return_value == 0)
        {
            /* block the parent if it's a fg job */
            if (!job->background)
            {
                list->fg_job = job;
                if (p_waitpid(job->pid, &job->status, false) == job->pid)
                {
                    remove_job(job, list, false);
                    free_job(job);
                    list->fg_job = NULL;
                }
            }
            else
            { /* else, won't block the parent */
                f_write(PENNOS_STDOUT, "Running: %s\n", 0, job->cmd);
            }
        }
    }
}
