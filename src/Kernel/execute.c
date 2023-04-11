#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // STDIN
#include <string.h> 

#include "execute.h"
#include "user.h"
#include "builtins.h"
#include "scheduler.h"

extern job_list *list;

/* execute the commands
* return = 0: success
* return = -1: failure
*/
int execute(struct parsed_command *cmd, job *job) {
    char *buf;
    int size;
    pid_t child = 0;
    // place holder

    if (strcmp(cmd->commands[0][0], "sleep") == 0) {
        size = strlen(cmd->commands[0][1])+1;
        buf = malloc(size);
        memcpy(buf, cmd->commands[0][1], size);

        char *sleep_arg[3] = {"sleep", buf, NULL};
        child = p_spawn(my_sleep, sleep_arg, 1, STDIN_FILENO, STDOUT_FILENO);
        // free(buf);
    } else if (strcmp(cmd->commands[0][0], "zombify") == 0) {
        char *zombie_arg[2] = {"zombify", NULL};
        child = p_spawn(zombify, zombie_arg, 0, STDIN_FILENO, STDOUT_FILENO);
    
    } else if (strcmp(cmd->commands[0][0], "orphanify") == 0) {
        char *orphan_arg[2] = {"orphanify", NULL};
        child = p_spawn(orphanify, orphan_arg, 0, STDIN_FILENO, STDOUT_FILENO);
    } else {
        /* invalid input */
        free_job(job);
        return -1;
    }

    job->pid = child;
    job->pgid = child;
    return 0;
}