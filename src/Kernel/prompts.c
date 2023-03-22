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