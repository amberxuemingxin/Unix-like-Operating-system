#include "shell.h"
#include "parser.h"

int loop () {
    
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

        free(cmd);
    }

    // call scheduler 
    return 1;
}