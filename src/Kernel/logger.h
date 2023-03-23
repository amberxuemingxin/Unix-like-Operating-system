#include <stdio.h>
#include <stdlib.h>

/* Macros for the logger */
#define CREATE 0
#define SIGNALED 1
#define EXITED 2
#define ZOMBIE 3
#define ORPHAN 4
#define WAITED 5

void log_events(int type, int ticks, int pid, int priority, char *process);