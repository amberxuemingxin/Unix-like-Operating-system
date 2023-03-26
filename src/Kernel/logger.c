#include "logger.h"

void log_events(int type, int ticks, int pid, int priority, char *process) {
    char *log_type;

    switch (type)
    {
    case 0:
        log_type = "CREATE";
        break;
    case 1: 
        log_type = "SIGNALED";
        break;
    case 2:
        log_type = "EXITED";
        break;
    case 3:
        log_type = "ZOMBIE";
        break;
    case 4:
        log_type = "ORPHAN";
        break;
    case 5:
        log_type = "WAITED";
        break;
    case 6:
        log_type = "SCHEDULE";
        break;
    case 8:
        log_type = "NICE";
        break;
    case 9:
        log_type = "BLOCKED";
        break;
    case 10:
        log_type = "UNBLOCKED";
        break;
    case 11:
        log_type = "STOPPED";
        break;
    case 12:
        log_type = "CONTINUED";
        break;
    default:
        log_type = "";
        break;
    }

    // output file for log
    FILE *log_file = fopen("log/log.txt", "a+");
    if (log_file== NULL) {
        perror("Fail to create the log file.\n");
        exit(EXIT_FAILURE);
    }
    int return_value = fprintf(log_file, "[%d]\t%s\t%d\t%d\t%s\n", ticks, log_type, pid, priority, process);
    if (return_value < 0) {
        perror("Fail to create the log file.\n");
        exit(EXIT_FAILURE);
    }
    fclose(log_file);
}