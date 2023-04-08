#include <stdio.h>
#include <stdlib.h>

/* Macros for the logger */
#define CREATE 0
#define SIGNALED 1
#define EXITED 2
#define ZOMBIE 3
#define ORPHAN 4
#define WAITED 5
#define SCHEDULE 6
#define BLOCKED 7
#define UNBLOCKED 8
#define STOPPED 9
#define CONTINUED 10

char *time_stamp();

void log_events(int type, int ticks, int pid, int priority, char *process);

void log_nice(int ticks, int pid, int old_priority, int new_priority, char *process);

void free_logger();