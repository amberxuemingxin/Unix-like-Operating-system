#include <stdio.h>

#include "kernel.h"
#include "scheduler.h"
#include "ucontext.h"

// ucontext_t main_context;
ucontext_t scheduler_context;
ucontext_t idle_context;

// set up signals handling exit ctrl c

int main(int argc, char *argv[]) {

    // initialize the scheduler
    init_scheduler();

    getcontext(&scheduler_context);

    // create the context for scheduler
    char *scheduler_args[2] = {"schedule", NULL};
    // void func(int a, int b)
    // args = {"2", "a b"}
    make_context(&scheduler_context, &schedule, scheduler_args);

    /* init the context for the idle process */
    char *idle_args[2] = {"idle", NULL};
    make_context(&idle_context, &idle_process, idle_args);

    set_alarm_handler();
    // set timer
    set_timer();

    // spawn a process for shell
    pcb_t *shell_process = k_shell_create();

    // add shell process into scheduler's ready queue
    node *shell = init_node(shell_process);

    add_to_scheduler(shell);

    setcontext(&scheduler_context);

    return 0;
}
