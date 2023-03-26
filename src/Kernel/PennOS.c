#include <stdio.h>

#include "kernel.h"
#include "scheduler.h"

scheduler *s;

// ucontext_t main_context;
ucontext_t scheduler_context;

// set up signals handling exit ctrl c

int main(int argc, char *argv[]) {

    // initialize the scheduler
    s = init_scheduler();

    char *scheduler_arg[2] = {"schedule", NULL};
    // make_context(&scheduler_context, scheduler)

    // create the context for scheduler
    make_context(&scheduler_context, schedule, scheduler_arg);
    set_alarm_handler();
    // set timer
    set_timer();

    // spawn a process for shell
    pcb_t *shell_process = k_shell_create();

    // add shell process into scheduler's ready queue
    node *shell = init_node(shell_process);

    add_to_scheduler(shell, s);

    setcontext(&scheduler_context);

    return 0;
}
