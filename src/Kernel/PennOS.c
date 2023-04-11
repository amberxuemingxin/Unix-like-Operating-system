#include <stdio.h>
#include <signal.h>

#include "kernel.h"
#include "scheduler.h"
#include "ucontext.h"
#include "user.h"
#include "logger.h"
#include "shell.h"

ucontext_t scheduler_context;
ucontext_t idle_context;
ucontext_t exit_context;
bool idle;

extern pcb_t *active_process;
extern char *log_name;

void sigint_handler(int signo) {
    if (active_process && active_process->pid != 1) {
        k_process_kill(active_process, S_SIGNALED);
    }
}

void sigtstp_handler(int signo) {
    if (active_process && active_process->pid != 1) {
        k_process_kill(active_process, S_SIGSTOP);
    }
}

// set up signals handling exit ctrl c
/*
 * the main PennOS function that calls other functions and initialize the entire project
 */
int main(int argc, char *argv[])
{

    // initialize the scheduler
    init_scheduler();

    log_name = time_stamp();

    // getcontext(&main_context);
    getcontext(&scheduler_context);

    // create the context for scheduler
    char *scheduler_args[2] = {"schedule", NULL};

    make_context(&scheduler_context, &schedule, 0, scheduler_args);

    // init the context for the idle process
    char *idle_args[2] = {"idle", NULL};
    make_context(&idle_context, &idle_process, 0, idle_args);

    char *exit_args[2] = {"exit", NULL};
    make_context(&exit_context, &exit_process, 0, exit_args);

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
    set_alarm_handler();
    // set timer
    set_timer();

    // spawn a process for shell
    char *shell_args = "shell";
    p_spawn(shell_loop, &shell_args, 0, STDIN_FILENO, STDOUT_FILENO);

    idle = true;
    setcontext(&idle_context);
    return 0;
}
