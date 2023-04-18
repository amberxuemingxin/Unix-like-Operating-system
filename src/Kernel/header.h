#ifndef HEADER_H
#define HEADER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>            // STDIN_FILENO
#include <ucontext.h>          // getcontext, makecontext, setcontext, swapcontext

/**
 * @{ \name Signals
 */
#define S_SIGSTOP 0 /**< Signal of stopping the process. */
#define S_SIGCONT_FG 1 /**< Signal of continuing the process in foreground. */
#define S_SIGCONT_BG 2 /**< Signal of continuing the process in background. */
#define S_SIGTERM 3 /**< Signal of terminating the process. */
#define S_SIGNALED 4 /**< Signal of terminating the process by user input. */
/**
 * @}
 */

#endif