#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "FAT.h"
#include "macro.h"
void signalHandler(int sigNum) {
    if (sigNum == SIGINT) {
        ssize_t numBytes = write(STDERR_FILENO, "\n", 1);
        if (numBytes == -1) {
            perror("write");
        }
        numBytes = write(STDERR_FILENO, PENNFAT_PROMPT, PROMPT_LENGTH);
        if (numBytes == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }
}


int main(int argc, char *argv[]) {
    // FAT *currFat = NULL;

    // bind signal handler for sigint
    if (signal(SIGINT, signalHandler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
    while (1) {
        ssize_t numBytes = write(STDERR_FILENO, PENNFAT_PROMPT, PROMPT_LENGTH);
        if (numBytes == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        char *line = NULL;
        size_t len = 0;
        ssize_t n;

        n = getline(&line, &len, stdin);

        // handle error
        if(n == -1) {
            if(line!=NULL) {
                free(line);
            }
            exit(EXIT_FAILURE);
        }
        if (n == 1 && line[n - 1] == '\n') {
            if (line != NULL) {
            free(line);
            }
        } else {
            printf("execute %s\n", line);
        }
        free(line);
    }
}