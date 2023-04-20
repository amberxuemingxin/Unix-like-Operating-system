#include <unistd.h>

#include "execute.h"
#include "user.h"
#include "builtins.h"
#include "scheduler.h"
#include "stress.h"
#include "../pennFAT/FAT.h"
#include "../pennFAT/macro.h"
#include "../pennFAT/pennfatlib.h"

extern job_list *list;
extern bool file_system;

// void my_truncate(char ***arr)
// {
//     (*arr) += 1; // increment the pointer by 2
//     // now (*arr) points to the third element of the original array
// }

// int redirect_in(const char *filename, job *j)
// {
//     int fd_0;
//     if ((fd_0 = open(filename, O_RDONLY, 0644)) < 0)
//     {
//         remove_job(j, list, false);
//         free_job(j);
//         return FAILURE;
//     }

//     j->fd0 = fd_0;
// }

// void redirect_out(const char *filename, bool is_append, job *j)
// {
//     int fd_1;

//     // standard output redirection
//     if (!is_append)
//     {
//         if ((fd_1 = creat(filename, 0644)) < 0)
//         {
//             remove_job(j, list, false);
//             free_job(j);
//             return FAILURE;
//         }
//         // append output redirection
//     }
//     else
//     {
//         if ((fd_1 = open(filename, O_RDWR | O_APPEND, 0644)) < 0)
//         {
//             remove_job(j, list, false);
//             free_job(j);
//             return FAILURE;
//         }
//     }

//     j->fd1 = fd_1;
// }

/* execute the commands
* return = 0: success
* return = -1: failure
*/
int execute(struct parsed_command *cmd, job *j, int priority) {
    char *buf;
    int size;
    pid_t child = 0;
    

    // file redirection
    // if (cmd->stdin_file) {
    //     redirect_in(cmd->stdin_file, j);
    // }

    // if (cmd->stdout_file) {
    //     redirect_out(cmd->stdout_file, cmd->is_file_append, j);
    // }

    if (strcmp(cmd->commands[0][0], "sleep") == 0) {
        if (!cmd->commands[0][1]) {
            f_write(PENNOS_STDOUT, "Please input an argument for sleep!\n", 0);
            remove_job(j, list, false);
            free_job(j);
            return FAILURE;
        }
        size = strlen(cmd->commands[0][1])+1;
        buf = malloc(size);
        memcpy(buf, cmd->commands[0][1], size);

        char *sleep_arg[3] = {"sleep", buf, NULL};
        child = p_spawn(my_sleep, (void *)sleep_arg, 1, j->fd0, j->fd1, priority, cmd->is_background);
    } else if (strcmp(cmd->commands[0][0], "zombify") == 0) {
        char *zombie_arg[2] = {"zombify", NULL};
        child = p_spawn(zombify, (void *)zombie_arg, 0, j->fd0, j->fd1, priority, cmd->is_background);
    } else if (strcmp(cmd->commands[0][0], "orphanify") == 0) {
        char *orphan_arg[2] = {"orphanify", NULL};
        child = p_spawn(orphanify, (void *)orphan_arg, 0, j->fd0, j->fd1, priority, cmd->is_background);
    } else if (strcmp(cmd->commands[0][0], "ps") == 0) {
        char *ps_arg[2] = {"ps", NULL};
        child = p_spawn(ps, (void *)ps_arg, 0, j->fd0, j->fd1, priority, cmd->is_background);
    } else if (strcmp(cmd->commands[0][0], "hang") == 0) {
        char *hang_arg[2] = {"hang", NULL};
        child = p_spawn(hang, (void *)hang_arg, 0, j->fd0, j->fd1, priority, cmd->is_background);
    } else if (strcmp(cmd->commands[0][0], "nohang") == 0) {
        char *nh_arg[2] = {"nohang", NULL};
        child = p_spawn(nohang, (void *)nh_arg, 0, j->fd0, j->fd1, priority, cmd->is_background);
    } else if (strcmp(cmd->commands[0][0], "recur") == 0) {
        char *recur_arg[2] = {"recur", NULL};
        child = p_spawn(recur, (void *)recur_arg, 0, j->fd0, j->fd1, priority, cmd->is_background);
    } else if (strcmp(cmd->commands[0][0], "busy") == 0) {
        char *busy_arg[2] = {"busy", NULL};
        child = p_spawn(busy, (void *)busy_arg, 0, j->fd0, j->fd1, priority, cmd->is_background);
    } else if (strcmp(cmd->commands[0][0], "kill") == 0) {
        if (!cmd->commands[0][1] || !cmd->commands[0][2]) {
            f_write(PENNOS_STDOUT, "Please input an argument for kill [signal] [pid]!\n", 0);
            remove_job(j, list, false);
            free_job(j);
            return FAILURE;
        }

        char *kill_arg[4] = {"kill", cmd->commands[0][1], cmd->commands[0][2], NULL};
        int pid_num = atoi(cmd->commands[0][2]);
        if (pid_num == 0)
        {
            f_write(PENNOS_STDOUT, "Fail to atoi\n", 0);
            remove_job(j, list, false);
            free_job(j);
            return FAILURE;
        } else if (pid_num == 1 ) {
            remove_job(j, list, false);
            free_job(j);
            return FAILURE;
        }

        if (strcmp(cmd->commands[0][1], "term") == 0) {
            /* kill the job */
            job *job_to_remove = find_by_pid(pid_num, list);
            remove_job(job_to_remove, list, true);
            remove_job(job_to_remove, list, false);
            free_job(job_to_remove);
        }

        /* kill the process */
        child = p_spawn(my_kill, (void *)kill_arg, 2, j->fd0, j->fd1, priority, cmd->is_background);
    } else if (file_system && strcmp(cmd->commands[0][0], "cat") == 0) {
        char *arg_name = "cat";
        void *cat_arg[2] = {arg_name, cmd->commands[0]};
        child = p_spawn((void *)pennfat_cat, cat_arg, 1, j->fd0, j->fd1, priority, cmd->is_background);
    } else if (file_system && strcmp(cmd->commands[0][0], "echo") == 0) {
        // TODO

    } else if (file_system && strcmp(cmd->commands[0][0], "ls") == 0) {
        char *ls_arg[2] = {"ls", NULL};
        child = p_spawn((void *)pennfat_ls, (void *)ls_arg, 0, j->fd0, j->fd1, priority, cmd->is_background);
    } else if (file_system && strcmp(cmd->commands[0][0], "touch") == 0) {
        char *arg_name = "touch";
        void *touch_arg[2] = {arg_name, cmd->commands[0]};
        child = p_spawn((void *)pennfat_touch, touch_arg, 1, j->fd0, j->fd1, priority, cmd->is_background);
        // parse_pennfat_command(cmd->commands, cmd->num_commands);
    } else if (file_system && strcmp(cmd->commands[0][0], "mv") == 0) {
        if (!cmd->commands[0][1] || !cmd->commands[0][2]) {
            f_write(PENNOS_STDOUT, "Please input an argument for mv!\n", 0);
            remove_job(j, list, false);
            free_job(j);
            return FAILURE;
        }
        char *mv_arg[4] = {"mv", cmd->commands[0][1], cmd->commands[0][2], NULL};
        child = p_spawn((void *)pennfat_mv, (void *)mv_arg, 2, j->fd0, j->fd1, priority, cmd->is_background);
    } else if (file_system && strcmp(cmd->commands[0][0], "cp") == 0) {
        char *arg_name = "cp";
        void *cp_arg[2] = {arg_name, cmd->commands[0]};
        child = p_spawn((void *)pennfat_cp, cp_arg, 1, j->fd0, j->fd1, priority, cmd->is_background);
    } else if (file_system && strcmp(cmd->commands[0][0], "rm") == 0) {
        char *arg_name = "rm";
        void *rm_arg[2] = {arg_name, cmd->commands[0]};
        child = p_spawn((void *)pennfat_remove, rm_arg, 1, j->fd0, j->fd1, priority, cmd->is_background);
    } else if (file_system && strcmp(cmd->commands[0][0], "chmod") == 0) {
        char *arg_name = "chmod";
        void *ch_arg[2] = {arg_name, cmd->commands[0]};
        child = p_spawn((void *)pennfat_chmod, ch_arg, 1, j->fd0, j->fd1, priority, cmd->is_background);
    } else {
        /* invalid input */
        remove_job(j, list, false);
        free_job(j);
        return FAILURE;
    }

    j->pid = child;
    j->pgid = child;
    return 0;
}