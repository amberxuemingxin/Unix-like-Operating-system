#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // execvp
#include <string.h> 

#include "execute.h"
#include "user.h"
#include "built-ins/sleep.h"

void execute(struct parsed_command *cmd) {
    
    // place holder
    if (strcmp(cmd->commands[0][0], "sleep") == 0) {
        char *sleep_arg[3] = {cmd->commands[0][0], cmd->commands[0][1], NULL};
        printf("argv[0] = %s, argv[1] = %s\n", sleep_arg[0], sleep_arg[1]);
        pid_t child = p_spawn(my_sleep, sleep_arg, 1, STDIN_FILENO, STDOUT_FILENO);
        printf("pid == %d\n", child);
    }
}