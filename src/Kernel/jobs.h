#ifndef JOB_HEADER
#define JOB_HEADER

#include <sys/types.h>
#include "parser.h"

typedef struct jobs
{
    pid_t pid;
    pid_t pgid;

    char *cmd;
    bool background;
    int fd0;
    int fd1;

    int jid;
    int status;
    struct jobs *next;
} job;

typedef struct job_queue {
    struct jobs *queue_running;
    struct jobs *queue_stopped;
    struct jobs *fg_job;
    int max_jid;
} job_list;

job *init_job(struct parsed_command *cmd, job_list *list);

job_list *init_job_list();

job *find_by_jid(int jid, job_list *list);

void free_job(job *j);

void free_all_jobs(job_list *list);

void print_all_jobs(job_list *list);

void remove_job(job *j, job_list *list, bool stopped);

void add_to_head(job *j, job_list *list, bool stopped);

#endif
