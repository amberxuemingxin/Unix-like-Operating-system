/**
 * @file handler.h
 * @brief The command handler. Either prints out the user manual, or execute builtin
 * functions like "fg", "bg, and "nice"
 */
#include "parser.h"

void cmd_handler(struct parsed_command *cmd);