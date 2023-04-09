#include "shell.h"
#include "parser.h"
#include "kernel.h"
#include "execute.h"
#include "jobs.h"
#include "handler.h"

job_list *list;

void shell_loop () {
    list = init_job_list();
    
    while (1) {
        // prompt to the user
        // perror("before execution");
        shell_prompt();

        // get user input
        char *user_input = NULL;
        size_t size = 0;
        ssize_t chars_read;

        chars_read = getline(&user_input, &size, stdin);
        if (chars_read <= 0)
        {
            free(user_input);
            continue;
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

        if (cmd->num_commands >= 1) {
            cmd_handler(cmd);
        }
        
        // printf("%s\n", **cmd->commands);
        // perror("After execution");
        free(cmd);
    }
}