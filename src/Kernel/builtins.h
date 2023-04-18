/**
 * @file builtins.h
 * @brief Builtin functions that can be used by the user
 */

/**
 * @brief our own sleep function that sleeps for the ticks of time
 * @param the number of ticks the user wish to sleep
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

void my_kill(char *signo, char *pid);