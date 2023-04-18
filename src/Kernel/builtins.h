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