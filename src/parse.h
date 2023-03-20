struct parsed_command {
    bool is_backgroud;

    bool is_file_append;

    const char *stdin_file;

    const char *stdout_file;

    char *commands;
}