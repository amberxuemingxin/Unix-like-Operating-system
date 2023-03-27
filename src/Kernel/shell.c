#include "shell.h"
#include "parser.h"
#include "kernel.h"

#include "built-ins/ls.h"

void shell_loop () {
    
    while (1) {
        // prompt to the user
        shell_prompt();

        // get user input
        char *user_input = NULL;
        size_t size = 0;
        ssize_t chars_read;

        // exit
        chars_read = getline(&user_input, &size, stdin);
        if (chars_read <= 0)
        {
            free(user_input);
            
            break;
        }
        // parse user input
        struct parsed_command *cmd;
        int flag = parse_command(user_input, &cmd);

        
        free(user_input);

        if (flag < 0)
        {
            perror("invalid");
            continue;
        }
        if (flag > 0)
        {
            perror("invalid");
            continue;
        }

        if (strcmp(cmd->commands[0][0], "ls") == 0) {
            ls();
        }

        free(cmd);
    }
}