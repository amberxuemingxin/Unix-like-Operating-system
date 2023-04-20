#include <time.h>

#include "logger.h"

extern char *log_name;

char *time_stamp() {
    char *timestamp = (char *)malloc(sizeof(char) * 16);
    time_t ltime;
    ltime=time(NULL);
    struct tm *tm;
    tm=localtime(&ltime);

    sprintf(timestamp,"%04d%02d%02d%02d%02d%02d", tm->tm_year+1900, tm->tm_mon, 
        tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    return timestamp;
}

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
    case 7:
        log_type = "BLOCKED";
        break;
    case 8:
        log_type = "UNBLOCKED";
        break;
    case 9:
        log_type = "STOPPED";
        break;
    case 10:
        log_type = "CONTINUED";
        break;
    default:
        log_type = "";
        break;
    }

    char buffer[30]; // The filename buffer
    snprintf(buffer, sizeof(char) * 30, "log/log:%s.txt", log_name);

    // output file for log
    FILE *log_file = fopen(buffer, "a+");
    if (log_file== NULL) {
        perror("Fail to create the log file.\n");
        exit(EXIT_FAILURE);
    }
    int return_value = fprintf(log_file, "[\t%5d]\t%10s\t%5d\t%5d\t%s\n", ticks, log_type, pid, priority, process);
    if (return_value < 0) {
        perror("Fail to create the log file.\n");
        exit(EXIT_FAILURE);
    }
    fclose(log_file);
}

void log_nice(int ticks, int pid, int old_priority, int new_priority, char *process) {
    char buffer[32]; // The filename buffer
    snprintf(buffer, sizeof(char) * 32, "log/log%s.txt", log_name);

    // output file for log
    FILE *log_file = fopen(buffer, "a+");
    if (log_file== NULL) {
        perror("Fail to create the log file.\n");
        exit(EXIT_FAILURE);
    }
    int return_value = fprintf(log_file, "[\t%5d]\t%10s\t%5d\t%5d\t%5d\t%s\n", ticks, "NICE", pid, old_priority, new_priority,process);
    if (return_value < 0) {
        perror("Fail to create the log file.\n");
        exit(EXIT_FAILURE);
    }
    fclose(log_file);
}