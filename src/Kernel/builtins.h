/**
 * @file builtins.h
 * @brief Builtin functions that can be used by the user
 */

/**
 * @brief our own sleep function that sleeps for the ticks of time
 * @param ticks the number of ticks the user wish to sleep
 */
void my_sleep(char *ticks);

/**
 * @brief spawn a zombie process inside the current process
 */
void zombify();

/**
 * @brief spawn an orphan process inside the current process
 */
void orphanify();

/**
 * @brief print all processes
 */
void ps();

/**
 * @brief busy waiting
 */
void busy();

/**
 * @brief kill the process with the designated signal
 * @param signo the signal
 * @param pid the pid that the user wants to kill
 */
void my_kill(char *signo, char *pid);

/**
 * @brief echo the typed-in message back to the designated file descriptor
 * @param commands the commands wish to echo
 * @param fd0 the file descriptor to echo from
 * @param fd1 the file descriptor to echo to
 */
void my_echo(char **commands, int fd0, int fd1);