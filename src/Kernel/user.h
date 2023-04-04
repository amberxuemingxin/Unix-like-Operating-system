#include "PCB.h"

#define S_SIGSTOP 0
#define S_SIGCONT 1
#define S_SIGTERM 2

// // user level

// /*(U) forks a new thread that retains most of the attributes of the parent thread 
// (see k_process_create). Once the thread is spawned, it executes the function 
// referenced by func with its argument array argv.
// fd0 is the file descriptor for the input file, and fd1 is the file descriptor for 
// the output file. It returns the pid of the child thread on success, or -1 on error.*/
pid_t p_spawn(void (*func)(), char **argv, int num_arg, int fd0, int fd1);

void p_sleep(unsigned int ticks);

// void p_kill(pid_t pid, int sig);

// void p_waitpid(pid_t pid, int *wstatus, bool nohang);

void p_exit();

// int p_nice(pid_t pid, int priority);

// void p_sleep(unsigned int ticks);

// bool W_WIFCONTINUED(int *wstatus);

// bool W_WIFEXITED(int *wstatus);

// bool W_WIFSIGNALED(int *wstatus);

// bool W_WIFSTOPPED(int *wstatus);