#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "user.h"
#include "scheduler.h"
#include "../pennFAT/macro.h"
#include "../pennFAT/pennfatlib.h"

void my_sleep(void *arg)
{
    char *ticks = (char *)arg;

    int t = atoi(ticks);
    if (t == 0)
    {
        perror("Fail to atoi");
    }

    p_sleep(t * 10);
}

void zombie_child()
{
    return;
}

void zombify()
{
    char *zombie_arg[2] = {"zombie_child", NULL};
    p_spawn(zombie_child, (void *)zombie_arg, 0, STDIN_FILENO, STDOUT_FILENO, 0, false);
    while (1);
    return;
}

void orphan_child()
{
    while (1)
        ;
}

void orphanify()
{
    char *orphan_arg[2] = {"orphan_child", NULL};
    p_spawn(orphan_child, (void *)orphan_arg, 0, STDIN_FILENO, STDOUT_FILENO, 0, false);
    return;
}

void ps()
{
    print_all_process();
}

void busy()
{
    while (1)
        ;
}

void my_kill(char *signo, char *pid)
{
    int sig;
    // consider the signal
    if (strcmp(signo, "term") == 0)
    {
        sig = S_SIGTERM;
    }
    else if (strcmp(signo, "cont") == 0)
    {
        sig = S_SIGCONT_FG;
    }
    else if (strcmp(signo, "stop") == 0)
    {
        sig = S_SIGSTOP;
    }
    else
    {
        f_write(PENNOS_STDOUT, "Wrong signal!\n", 0);
        return;
    }

    int pid_num = atoi(pid);
    if (pid_num == 0)
    {
        f_write(PENNOS_STDOUT, "Fail to atoi\n", 0);
        return;
    }
    // call our p_kill function to kill the process
    p_kill(pid_num, sig);
}

void my_echo (char** commands, int *fd0, int *fd1) {
    char **buf = malloc(sizeof(char *));
    buf[0] = NULL;
    
    if (*fd0 == PENNOS_STDIN) {
        int i = 1;
        while (commands[i]) {
            buf[i-1] = commands[i];
            // printf("%d %s\n", i, commands[i]);
            i++;
            buf = realloc(buf, i * sizeof(char *));
        }

        buf[i-1] = NULL;
    } else {
        char *to_read = malloc(1); // allocate initial buffer of size 1
        int bytes_read = 0;
        int total_bytes_read = 0;
        bytes_read = f_read(*fd0, 1, to_read + total_bytes_read); // read 1 byte into buffer
        while(bytes_read != EOF)
        {
            total_bytes_read += bytes_read; // increment total bytes read
            if (total_bytes_read % 1024 == 0) { // increase buffer size in chunks of 1024 bytes
                to_read = realloc(to_read, total_bytes_read + 1024);
            }
            bytes_read = f_read(*fd0, 1, to_read + total_bytes_read); // read 1 byte into buffer
        }

        buf = realloc(buf, 2 * sizeof(char *));
        buf[0] = to_read;
        buf[1] = NULL;
    }

    int i = 0;
    //int return_value; //DEBUG
    // printf("%d\n", fd0);
    while (buf[i] != NULL) {
        if (i != 0) {
            // Print a space between arguments
            f_write(*fd1, " ", 0);
            // printf("return val = %d\n", return_value);
        }
        f_write(*fd1, "%s", 0, buf[i]);
        // printf("return val = %d\n", return_value);

        i++;
    }

    // Print a newline at the end
    f_write(*fd1, "\n", 0);
    // printf("return val = %d\n", return_value);

    if (*fd1 != PENNOS_STDOUT) {
        os_savefds();
        printf("yeah");
    }

}
