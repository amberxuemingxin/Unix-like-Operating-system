typedef struct parsed_command {
    bool is_backgroud;

    bool is_file_append;

    const char *stdin_file;

    const char *stdout_file;

    char *commands;
} parsed_command;

int parse_command(const char * const cmd_line, parsed_command **const result);

char *to_string(const parsed_command cmd);