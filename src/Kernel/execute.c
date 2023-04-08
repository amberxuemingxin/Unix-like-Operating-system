#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // execvp
#include <string.h> 

#include "execute.h"
#include "user.h"
#include "built-ins/sleep.h"

extern job_list *list;

void execute(struct parsed_command *cmd, job *job) {
    char *buf;
    int size;
    
    // place holder

    if (strcmp(cmd->commands[0][0], "sleep") == 0) {
        size = strlen(cmd->commands[0][1])+1;
        buf = malloc(size);
        memcpy(buf, cmd->commands[0][1], size);

        char *sleep_arg[3] = {"sleep", buf, NULL};
        pid_t child = p_spawn(my_sleep, sleep_arg, 1, STDIN_FILENO, STDOUT_FILENO);
        printf("pid == %d\n", child);
        // free(buf);
    }
}