#ifndef PCB_HEADER
#define PCB_HEADER

#include <sys/types.h>
#include "queue.h"
#include "parser.h"

typedef struct jobs
{
    pid_t pid;
    pid_t pgid;

    char *cmd;
    bool background;

    int jid;
    int status; /* RUNNING or STOPPED */
} job;

job *init_job(struct parsed_command *cmd, queue *q);



#endif
