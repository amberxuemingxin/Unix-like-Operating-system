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
    char *result;

    if (*fd0 == PENNOS_STDIN) {
        int i = 1;
        while (commands[i]) {
            f_write(*fd1, commands[i], strlen(commands[i]));
            f_write(*fd1, " ", 1);
            i++;
        }

        if (*fd1 == PENNOS_STDOUT) {
            f_write(*fd1, "\n", 1);
        } else {
            os_savefds();
        }

    } else {
        result = get_file_content(*fd0);
        if (result) {
            // f_write(PENNOS_STDOUT, result, strlen(result));
            f_write(*fd1, result, strlen(result));
        }

        if (*fd1 == PENNOS_STDOUT) {
            f_write(*fd1, "\n", 1);
        } else {
            os_savefds();
        }
    }

}
