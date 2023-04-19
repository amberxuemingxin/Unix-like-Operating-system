/**
 * @file user.h
 * @brief Contains all user level functions.
 */

#include "PCB.h"
#include "header.h"

/**
 * @{ \name User Level Functions for Process Status
 */
#define W_WIFEXITED(wstatus) (wstatus == EXITED_P || wstatus == ZOMBIED_P) /**< Return true if the process is considered dead. That is, the process is either exited normally or being a zombie. */
#define W_WIFSIGNALED(wstatus) (wstatus == EXITED_P) /**< Return true if the process is terminated by user's input. */
#define W_WIFSTOPPED(wstatus) (wstatus == STOPPED_P) /**< Return true if the procesw is stopped. */

/**
 * @brief Spawn a process.
 * 
 * @param func The function of the process.
 * @param argv A list of char array as the arguments of the function.
 * @param num_arg The number of arguments of the function.
 * @param fd0 The file descriptor for stdin.
 * @param fd1 The file descriptor for stdout.
 * @param priority The nice value of the process.
 * @param background If the process is a background process.
 * @return A pointer to the spawned process.
*/
pid_t p_spawn(void (*func)(), void *argv[], int num_arg, int fd0, int fd1, int priority, bool background);

/**
 * @brief A function that will stay idle for the specified amount of ticks.
 * 
 * @param ticks The number of seconds that the function need to stay idle.
*/
void p_sleep(unsigned int ticks);

/**
 * @brief Sends the specified signal to the destination process.
 * 
 * @param pid The destination process.
 * @param sig The signals. Can be S_SIGCONT, S_SIGTERM, S_SIGSTOP.
 * @return 0 if success, -1 if the pid is not existing.
*/
int p_kill(pid_t pid, int sig);

/**
 * @brief Wait for the specified children; if any of them are finished, clean up the finished children.
 * 
 * @param pid The children's pid. If pid == -1, the function will wait for every children of the caller process.
 * @param wstatus A pointer point to the children's status.
 * @param nohang Option of hanging or not. If nohang, the function will return the result immediately; else, the function will block the caller process until any children have changed the status.
 * @return The pid of the child that's been waited successfully. The function will also return 0 if no child has changed the status, and -1 if the caller has no child. 
*/
pid_t p_waitpid(pid_t pid, int *wstatus, bool nohang);

/**
 * @brief Exit the caller process gracefully.
*/
void p_exit();

/**
 * @brief Change the nice value of the specified process.
 * 
 * @param pid The destination process.
 * @param priority The new nice value.
 * @return 0 if success, -1 if the process not exists.
*/
int p_nice(pid_t pid, int priority);