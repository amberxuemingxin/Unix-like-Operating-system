#include <stdio.h>

#include "kernel.h"
#include "scheduler.h"
#include "ucontext.h"
#include "user.h"
#include "logger.h"
#include "shell.h"

ucontext_t scheduler_context;
ucontext_t idle_context;
bool idle;

extern char *log_name;

// set up signals handling exit ctrl c
/*
 * the main PennOS function that calls other functions and initialize the entire project
 */
int main(int argc, char *argv[])
{

    // initialize the scheduler
    init_scheduler();

    log_name = time_stamp();

    getcontext(&scheduler_context);

    // create the context for scheduler
    char *scheduler_args[2] = {"schedule", NULL};

    // void func(int a, int b)
    // args = {"2", "a b"}
    make_context(&scheduler_context, &schedule, scheduler_args);

    // init the context for the idle process
    char *idle_args[2] = {"idle", NULL};
    make_context(&idle_context, &idle_process, idle_args);

    set_alarm_handler();
    // set timer
    set_timer();

    // spawn a process for shell
    char *shell_args = "shell";
    p_spawn(shell_loop, &shell_args, STDIN_FILENO, STDOUT_FILENO);

    idle = true;
    setcontext(&idle_context);
    return 0;
}
