#include "handler.h"
#include "jobs.h"
#include "execute.h"
#include "user.h"
#include "queue.h"
 
extern job_list *list;
extern queue *queue_block;
extern pcb_t *active_process; 
extern ucontext_t scheduler_context;
int priority = 0;

void my_truncate(char*** arr) {
    (*arr) += 2;  // increment the pointer by 2
    // now (*arr) points to the third element of the original array
}

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

        // if the job is stopped, resume it
        if (job->status == STOPPED_P)
        {
            // TODO resume_job(job);
            fprintf(stderr, "Restarting: %s\n", job->cmd);
            job->status = RUNNING_P;
            list->fg_job = job;
            job->background = false;
            remove_job(job, list, true);
            add_to_head(job, list, false);
            p_kill(job->pid, S_SIGCONT);
        }
        else
        {
            fprintf(stderr, "%s\n", job->cmd);
        }

        // bring job to fg
        swapcontext(&active_process->context, &scheduler_context);

        // wait for fg
        if (p_waitpid(job->pid, &job->status, false) == job->pid) {
            remove_job(job, list, false);
            free_job(job);
            list->fg_job = NULL;
        }

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

        p_kill(job->pid, S_SIGCONT);
        job->status = RUNNING_P;
        remove_job(job, list, true);
        add_to_head(job, list, false);
        fprintf(stderr, "Running: %s\n", job->cmd);

        return;
        
    } else if (strcmp(cmd->commands[0][0], "nice") == 0) {
        char *priority_string = cmd->commands[0][1];

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

        my_truncate(cmd->commands);

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
        return;
    } else if (strcmp(cmd->commands[0][0], "man") == 0) {
        printf("USER MANUAL OF PENNOS\n");
        printf("-------------------------------------------\n");
        printf("Shell Built-ins: (args with * are optional)\n");
        printf("  cat [args]*\t\t");
        printf("reads data from the file and gives their content as output\n");
        printf("  sleep [args]\t\t");
        printf("causes the calling thread to sleep [args] seconds; args should be some integers\n");
        printf("  busy\t\t\t");
        printf("waits indefinitely\n");
        printf("  echo [args]*\t\t");
        printf("displays a line of text from [args]\n");
        printf("  ls [args]*\t\t");
        printf("lists all files in the directory\n");
        printf("  touch [args]\t\t");
        printf("creates an empty file if it does not exist, or update its timestamp otherwise\n");
        printf("  mv [arg1] [arg2]\t");
        printf("rename arg1 to arg2\n");
        printf("  cp [arg1] [arg2]\t");
        printf("copy arg1 to arg2\n");
        printf("  rm [args]\t\t");
        printf("remove [args] from the directory\n");
        printf("  chmod [args]\t\t");
        printf("changes the file mode bits of each given file according to mode\n");
        printf("  ps\t\t\t");
        printf("lists all processes on PennOS\n");
        printf("  kill [arg1] [arg2]\t");
        printf("sends a specified signal [arg1] to the specified process [arg2]\n");
        return; 
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
        int return_value = execute(cmd, job, priority);
        priority = 0;

        /* if execution success */
        if (return_value == 0) {
            /* block the parent if it's a fg job */
            if (!job->background) {
                list->fg_job = job;
                if (p_waitpid(job->pid, &job->status, false) == job->pid) {
                    remove_job(job, list, false);
                    free_job(job);
                    list->fg_job = NULL;
                }
            } else { /* else, won't block the parent */
                printf("Running: %s\n", job->cmd);
            }
        }
    }
}
