// #include <stdio.h>
// #include <string.h>
// #include "jobs.h"
// #include "PCB.h"

// const char STATUS[2][8] = {"running", "stopped"};

// // flatten the cmds into one-line command
// char *flatten(struct parsed_command *cmd)
// {
//     // calculate the size
//     int size = 0;
//     for (size_t i = 0; i < cmd->num_commands; i++)
//     {
//         for (char **arguments = cmd->commands[i]; *arguments != NULL; ++arguments)
//         {
//             size += strlen(*arguments);
//             size++; // add space after each cmd
//         }

//         if (i == 0 && cmd->stdin_file != NULL)
//         {
//             size += 2; // < + space
//             size += strlen(cmd->stdin_file);
//             size++;
//         }

//         if (i == cmd->num_commands - 1)
//         {
//             if (cmd->stdout_file != NULL)
//             {
//                 if (cmd->is_file_append)
//                 {
//                     size += 3; // >> + space
//                     size += strlen(cmd->stdout_file);
//                     size++;
//                 }
//                 else
//                 {
//                     size += 2; // > + space
//                     size += strlen(cmd->stdout_file);
//                     size++;
//                 }
//             }
//         }
//         else
//         {
//             size += 2;
//         }
//     }
//     if (cmd->is_background)
//     {
//         size++; // &
//     }
//     size++; // EOF

//     char *string = malloc(size * sizeof(char));
//     string[0] = '\0';

//     for (size_t i = 0; i < cmd->num_commands; ++i)
//     {
//         for (char **arguments = cmd->commands[i]; *arguments != NULL; ++arguments)
//         {
//             strcat(string, *arguments);
//             strcat(string, " ");
//         }

//         if (i == 0 && cmd->stdin_file != NULL)
//         {
//             strcat(string, "< ");
//             strcat(string, cmd->stdin_file);
//             strcat(string, " ");
//         }

//         if (i == cmd->num_commands - 1)
//         {
//             if (cmd->stdout_file != NULL)
//             {
//                 if (cmd->is_file_append)
//                 {
//                     strcat(string, ">> ");
//                 }
//                 else
//                 {
//                     strcat(string, "> ");
//                 }
//                 strcat(string, cmd->stdout_file);
//                 strcat(string, " ");
//             }
//         }
//         else
//         {
//             strcat(string, "| ");
//         }
//     }

//     return string;
// }

// job *init_job(struct parsed_command *cmd, int jid, pid_t pid, pid_t pgid, int fd0, int fd1) {
//     job *j = malloc(sizeof(job));
//     j->jid = jid;
//     j->pid = malloc(sizeof(pid_t) * cmd->num_commands);
//     j->pgid = pgid;
//     j->cmd = flatten(cmd);
//     j->background = cmd->is_background;
//     j->status = RUNNING_J;
//     j->fd0 = fd0;
//     j->fd1 = fd1;

//     int (*pipes)[2] = NULL;

//     if (cmd->num_commands > 1) {
//         pipes = malloc(sizeof(int[2]) * (cmd->num_commands - 1));
//     }

//     j->pipes = pipes;

//     return j;
// }

// job_list *init_job_list() {
//     job_list *list = malloc(sizeof(job_list));
//     list->max_jid = 0;
//     list->queue_running = init_queue();
//     list->queue_stopped = init_queue();

//     return list;
// }

// void free_job(job *j) {

// }

// void free_all_jobs(job_list *list) {

// }

// job *find_by_jid(int jid, job_list *list) {
//     pcb_t *tmp = list->queue_running->head;
//     while (tmp != NULL)
//     {
//         if (tmp_job->jid == jid)
//         {
//             return tmp_job;
//         }
//         tmp = tmp->next;
//     }

//     tmp = list->queue_stopped->head;
//     while (tmp != NULL)
//     {
//         job *tmp_job = (job *) tmp->payload;
//         if (tmp_job->jid == jid)
//         {
//             return tmp_job;
//         }
//         tmp = tmp->next;
//     }

//     // return null if not found
//     return NULL;
// }

// void print_all_jobs(job_list *list) {
//     for (int i = 1; i <= list->max_jid; i++)
//     {
//         job *curr = find_by_jid(i, list);
//         if (curr != NULL)
//         {
//             fprintf(stderr, "[%d] %s (%s)\n", curr->jid, curr->cmd, STATUS[curr->status]);
//         }
//     }
// }