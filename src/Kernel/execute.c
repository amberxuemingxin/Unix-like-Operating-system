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
        pid_t child = p_spawn(my_sleep, cmd->commands[0], STDIN_FILENO, STDOUT_FILENO);
        printf("pid == %d\n", child);
    }
}