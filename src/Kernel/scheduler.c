#include "scheduler.h"
#include "kernel.h"
#include "logger.h"
#include "PCB.h"
#include "user.h"
#include "jobs.h"

pcb_t *active_process;
pcb_t *active_sleep;

queue *queue_high;
queue *queue_mid;
queue *queue_low;
queue *queue_block;
queue *queue_zombie;

extern bool idle;
extern ucontext_t scheduler_context;
extern ucontext_t idle_context;
extern int global_ticks;
extern int max_pid;

void init_scheduler()
{
    queue_high = init_queue();
    queue_mid = init_queue();
    queue_low = init_queue();
    queue_block = init_queue();
    queue_zombie = init_queue();

    active_process = NULL;
    active_sleep = NULL;
}

void alarm_handler(int signum)
{
    if (signum == SIGALRM)
    {
        global_ticks++;
    }
    if (idle)
    {
        setcontext(&scheduler_context);
    }
    else
    {
        swapcontext(&(active_process->context), &scheduler_context);
    }
}

// setup the alarm handler that calls the scheduler every 100 ms
void set_alarm_handler()
{
    struct sigaction act;

    act.sa_handler = alarm_handler;
    act.sa_flags = SA_RESTART;
    sigfillset(&act.sa_mask);

    sigaction(SIGALRM, &act, NULL);
}

void set_timer()
{
    struct itimerval it;

    it.it_interval = (struct timeval){.tv_usec = CENTISECOND * 10};
    it.it_value = it.it_interval;

    setitimer(ITIMER_REAL, &it, NULL);
}

void add_to_scheduler(pcb_t *process)
{
    // add to different queue priority
    if (process->priority == -1)
    {
        add_process(queue_high, process);
    }
    else if (process->priority == 0)
    {
        add_process(queue_mid, process);
    }
    else if (process->priority == 1)
    {
        add_process(queue_low, process);
    }
}

int remove_from_scheduler(pcb_t *process)
{
    pcb_t *removed = NULL;

    if (process->priority == -1)
    {
        removed = remove_process(queue_high, process);
    }
    else if (process->priority == 0)
    {
        removed = remove_process(queue_mid, process);
    }
    else
    {
        removed = remove_process(queue_low, process);
    }

    if (removed == NULL)
    {
        return FAILURE;
    }
    else
    {
        return SUCCESS;
    }
}

// move the process from ready queue to block queue
void ready_to_block(pcb_t *process)
{
    remove_from_scheduler(process);
    add_process(queue_block, process);
}

// move the process from block queue to ready queue
void block_to_ready(pcb_t *process)
{
    remove_process(queue_block, process);
    add_to_scheduler(process);
}

// search through the entire linked list to find the designated PID
pcb_t *search_in_scheduler(pid_t pid)
{
    pcb_t *tmp = queue_high->head;

    while (tmp)
    {
        if (tmp->pid == pid)
        {
            return tmp;
        }
        tmp = tmp->next;
    }

    tmp = queue_mid->head;
    while (tmp)
    {
        if (tmp->pid == pid)
        {
            return tmp;
        }

        tmp = tmp->next;
    }

    tmp = queue_low->head;
    while (tmp)
    {
        if (tmp->pid == pid)
        {
            return tmp;
        }

        tmp = tmp->next;
    }

    tmp = queue_block->head;
    while (tmp)
    {
        if (tmp->pid == pid)
        {
            return tmp;
        }

        tmp = tmp->next;
    }

    return NULL;
}

// search through zombie processes to find the designated PID
pcb_t *search_in_zombies(pid_t pid)
{
    pcb_t *tmp = queue_zombie->head;
    while (tmp)
    {
        if (tmp->pid == pid)
        {
            return tmp;
        }

        tmp = tmp->next;
    }

    return NULL;
}

// This method decides the next process to run
pcb_t *pick_next_process()
{
    pcb_t *picked_process;

    bool low_queue_existed = (queue_low->length > 0) ? true : false;
    bool mid_queue_existed = (queue_mid->length > 0) ? true : false;
    bool high_queue_existed = (queue_high->length > 0) ? true : false;

    // if none of the queue has anything, switch to idel mode
    if (!low_queue_existed && !mid_queue_existed && !high_queue_existed)
    {
        idle = true;
        return NULL;
    }

    idle = false;
    // mid is 1.5 times more likely to be scheduled than low
    // high is 1.5 times more likely to be schedule than mid
    int low_length = queue_low->length;
    int mid_length = (int)(queue_mid->length * 1.5);
    int high_length = (int)(queue_high->length * 1.5 * 1.5);

    // pick a random one from all length combined
    int picked_queue = rand() % (low_length + mid_length + high_length);

    // see which queue it falls upon and choose from that queue if exists
    if (picked_queue < low_length && low_queue_existed)
    {
        picked_process = queue_low->head;
        remove_process(queue_low, picked_process);
        add_process(queue_low, picked_process);
    }
    else if (picked_queue < low_length + mid_length && mid_queue_existed)
    {
        picked_process = queue_mid->head;
        remove_process(queue_mid, picked_process);
        add_process(queue_mid, picked_process);
    }
    else
    {
        picked_process = queue_high->head;
        remove_process(queue_high, picked_process);
        add_process(queue_high, picked_process);
    }

    return picked_process;
}

// main schedule process that would run every 100 ms
void schedule()
{
    set_timer();

    // decrement for all sleeps
    pcb_t *sleep_process = queue_block->head;
    while (sleep_process && sleep_process->status != STOPPED_P && sleep_process->ticks >= 0)
    {
        sleep_process->ticks--;
        // if the sleep process is no longer in the background, switch to active
        if (!sleep_process->background)
        {
            active_sleep = sleep_process;
        }
        // if the tick reaches -1, unblock the process that's sleeping
        if (sleep_process->ticks == -1)
        {
            k_unblock(sleep_process);
        }
        sleep_process = sleep_process->next;
    }

    pcb_t *next_process = pick_next_process();
    // if there's no next process, switch the context to idel
    if (next_process == NULL)
    {
        setcontext(&idle_context);
        perror("setcontext - idle");
        exit(EXIT_FAILURE);
    }
    // otherwise schedule for the next process
    if (active_process != next_process)
    {
        active_process = next_process;
        log_events(SCHEDULE, global_ticks, active_process->pid, active_process->priority, active_process->process);
    }

    // setcontext process->context
    setcontext(&(active_process->context));

    // end of scheduler
    perror("setcontext");
    exit(EXIT_FAILURE);
}

void exit_scheduler()
{
    free(queue_high);
    free(queue_mid);
    free(queue_low);
    free(queue_block);
    free(queue_zombie);
}

void print_all_process()
{
    printf("%8s%8s%8s%8s\t%s\n", "PID", "PPID", "PRI", "STAT", "CMD");

    for (pid_t i = 0; i <= max_pid; i++)
    {
        pcb_t *p = search_in_scheduler(i) ? search_in_scheduler(i) : search_in_zombies(i);

        if (p)
        {
            char *status = malloc(2 * sizeof(char));
            if (p->status == STOPPED_P)
            {
                status = "S"; /* STOPPED */
            }
            else if (p->status == BLOCKED_P && strcmp(p->process, "sleep") != 0)
            {
                status = "B"; /* BLOCKED */
            }
            else if (p->status == ZOMBIED_P)
            {
                status = "Z"; /* ZOMBIED */
            }
            else
            {
                status = "R"; /* RUNNING */
            }
            printf("%8d%8d%8d%8s\t%s\n", p->pid, p->ppid, p->priority, status, p->process);
        }
    }
}