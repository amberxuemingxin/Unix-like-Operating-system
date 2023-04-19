#include "kernel.h"
#include "scheduler.h"
#include "user.h"
#include "logger.h"
#include "shell.h"
#include "./pennFAT/FAT.h"
#include "./pennFAT/macro.h"
#include "./pennFAT/pennfatlib.h"


ucontext_t scheduler_context;
ucontext_t idle_context;
ucontext_t exit_context;
bool idle;

extern char *log_name;

// set up signals handling exit ctrl c
/*
 * the main PennOS function that calls other functions and initialize the entire project
 */
int main(int argc, char *argv[])
{
    //// SHUFAN
    char* f_name = argv[1];
    struct parsed_command* parsed_cmd = NULL;
    char *cmd = "unmount";
    parse_command(cmd, &parsed_cmd);
    //unmount curr_fat first    
    if(parse_pennfat_command(parsed_cmd->commands,1)==FAILURE) {
        printf("error: failed to unmount current file system");
    }
    //mount fat
    char* cmd_f_name = malloc(strlen(cmd) + strlen(f_name) + 2);
    sprintf(cmd_f_name, "%s %s", cmd, f_name);
    parse_command(cmd_f_name, &parse_command);
    if(parse_pennfat_command(parsed_cmd->commands,2)==FAILURE) {
        printf("error: failed to mount %s\n", f_name);
    }
    //// SHUFAN

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

    set_alarm_handler();
    // set timer
    set_timer();

    // spawn a process for shell
    char *shell_args = "shell";
    p_spawn(shell_loop, &shell_args, 0, STDIN_FILENO, STDOUT_FILENO, -1, false);

    idle = true;
    setcontext(&idle_context);
    return 0;
}
