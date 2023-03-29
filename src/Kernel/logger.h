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
#define NICE 7
#define BLOCKED 8
#define UNBLOCKED 9
#define STOPPED 10
#define CONTINUED 11

char *time_stamp();

void log_events(int type, int ticks, int pid, int priority, char *process);