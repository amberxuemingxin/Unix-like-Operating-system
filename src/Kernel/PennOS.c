#include <stdio.h>

#include "kernel.h"
#include "scheduler.h"

scheduler *s;

extern ucontext_t main_context;
extern ucontext_t scheduler_context;

// set up signals handling exit ctrl c

int main(int argc, char *argv[]) {

    // initialize the scheduler
    s = init_scheduler();

    // char *scheduler_arg[2] = {"scheduler", NULL};
    // make_context(&scheduler_context, scheduler)

    // create the context for scheduler

    // set timer
    set_timer();

    // spawn a process for shell
    pcb_t *shell_process = k_shell_create();

    // add shell process into scheduler's ready queue
    node *shell = init_node(shell_process);
    add_node(s->queue_high, shell);

    setcontext(&(shell_process)->context);

    return 0;
}
