#include <signal.h>

#include "shell.h"
#include "parser.h"
#include "kernel.h"
#include "user.h"
#include "execute.h"
#include "jobs.h"
#include "handler.h"

job_list *list;
extern pcb_t *active_process;
extern pcb_t *active_sleep;

void sigint_handler(int signo) {
    if (active_sleep) {
        k_process_kill(active_sleep, S_SIGNALED);
    } else if (active_process && active_process->pid != 1) {
        k_process_kill(active_process, S_SIGNALED);
    }
}

void sigtstp_handler(int signo) {
    if (active_sleep) {
        k_process_kill(active_sleep, S_SIGSTOP);
    } else if (active_process && active_process->pid != 1) {
        k_process_kill(active_process, S_SIGSTOP);
    }

    job *j = list->fg_job;
    j->status = STOPPED_P;

    remove_job(j, list, false);
    add_to_head(j, list, true);

    list->fg_job = NULL;

    printf("Stopped: %s\n", j->cmd);
}

void shell_loop () {
    list = init_job_list();

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
    
    while (1) {
        /* checking for bg */
        job *j = list->queue_running;

        while (j) {
            job *tmp = j->next;
            if (j->background) {
                int return_value = p_waitpid(j->pid, &j->status, true);
                if (return_value == j->pid) {
                    remove_job(j, list, false);
                    printf("Finished: %s\n", j->cmd);
                    free_job(j);
                }
            }
            j = tmp;
        }

        // first prompt to the user
        shell_prompt();

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

        if (cmd->num_commands >= 1) {
            cmd_handler(cmd);
        }
        
        free(cmd);
    }
}