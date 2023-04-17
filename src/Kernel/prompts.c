#include "prompts.h"

const char *PROMPT = "$ ";

void shell_prompt() {
    int return_value = write(STDERR_FILENO, PROMPT, strlen(PROMPT));

    if ( return_value == -1)
    {
        perror("Fail to write!\n");
        exit(EXIT_FAILURE);
    }
}

char *cat_usage() {
    return "reads data from the file and gives their content as output";
}

// char *sleep_usage() {

// }

// char *busy_usage();

// char *echo_usage();

// char *ls_usage();

// char *touch_usage();

// char *mv_usage();

// char *cp_usage();

// char *rm_usage();

// char *chmod_usage();

// char *ps_usage();

// char *kill_usage();

// char *zombify_usage();

// char *orphanify_usage();

// char *nice_usage();

// char *nice_pid_usage();

// char *man_usage();

// char *bg_usage();

// char *fg_usage();

// char *jobs_usage();

// char *logout_usage();