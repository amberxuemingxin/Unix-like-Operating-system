/**
 * @file shell.h
 * @brief The main shell loop that keeps blocking until receives user input
 * then parses it and transfers the commands to the command handler.
 * It also handles sigint, and sigtstop
 *
 */
#include "kernel.h"
/**
 * @brief The main shell loop
 */
void shell_loop();