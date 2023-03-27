#include <sys/time.h>  // setitimer
#include <stdlib.h> // rand
#include <signal.h> // sigaction, sigemptyset, sigfillset, signal

#include "scheduler.h"
#include "kernel.h"
#include "logger.h"
#include "PCB.h"

node *active_node;

queue *queue_high;
queue *queue_mid;
queue *queue_low;
queue *queue_sleep;
bool idle;

extern ucontext_t scheduler_context;
extern ucontext_t idle_context;
extern int ticks;

void init_scheduler() {
    queue_high = init_queue();
    queue_mid = init_queue();
    queue_low = init_queue();
    queue_sleep = init_queue();
}

void alarm_handler(int signum) {
    if (signum == SIGALRM) {
        ticks++;
    }
    if (idle) {
        setcontext(&scheduler_context);
    } else {
        pcb_t *active_process = (pcb_t *)active_node->payload;
        swapcontext(&(active_process->context), &scheduler_context);
    }
}

void set_alarm_handler() {
    struct sigaction act;

    act.sa_handler = alarm_handler;
    act.sa_flags = SA_RESTART;
    sigfillset(&act.sa_mask);

    sigaction(SIGALRM, &act, NULL);
}

void set_timer() {
    struct itimerval it;

    it.it_interval = (struct timeval) { .tv_usec = CENTISECOND * 10 };
    it.it_value = it.it_interval;

    setitimer(ITIMER_REAL, &it, NULL);
}

void add_to_scheduler(node *n) {
    pcb_t *process = (pcb_t *)n->payload;
    
    if (process->priority == -1) {
        add_node(queue_high, n);
    } else if (process->priority == 0) {
        add_node(queue_mid, n);
    } else if (process->priority == 1) {
        add_node(queue_low, n);
    }
}

void remove_from_scheduler(node *n) {
    pcb_t *process = (pcb_t *)n->payload;

    if (process->priority == -1) {
        remove_node(queue_high, n);
    } else if (process->priority == 0) {
        remove_node(queue_mid, n);
    } else if (process->priority == 1) {
        remove_node(queue_low, n);
    }
}

node *search_in_scheduler(pid_t pid) {
    node *tmp = queue_high->head;
    pcb_t *tmp_process;
    while (tmp) {
        tmp_process = (pcb_t *)tmp->payload;
        if (tmp_process->pid == pid) {
            return tmp;
        }

        tmp = tmp->next;
    }

    tmp = queue_mid->head;
    while (tmp) {
        tmp_process = (pcb_t *)tmp->payload;
        if (tmp_process->pid == pid) {
            return tmp;
        }

        tmp = tmp->next;
    }
    
    tmp = queue_low->head;
    while (tmp) {
        tmp_process = (pcb_t *)tmp->payload;
        if (tmp_process->pid == pid) {
            return tmp;
        }

        tmp = tmp->next;
    }

    return NULL;
}

node *pick_next_process() {
    node *picked_node;

    bool low_queue_existed = (queue_low->length > 0) ? true : false;
    bool mid_queue_existed = (queue_mid->length > 0) ? true : false;
    bool high_queue_existed = (queue_high->length > 0) ? true : false;

    if (!low_queue_existed && !mid_queue_existed && !high_queue_existed) {
        idle = true;
        return NULL;
    }

    idle = false;
    int low_length = queue_low->length;
    int mid_length = (int) (queue_mid->length * 1.5);
    int high_length = (int) (queue_high->length * 1.5 * 1.5);

    int picked_queue = rand() % (low_length + mid_length + high_length);
    
    if (picked_queue < low_length && low_queue_existed) {
        picked_node = queue_low->head;
    } else if (picked_queue < low_length + mid_length && mid_queue_existed) {
        picked_node = queue_mid->head;
    } else {
        picked_node = queue_high->head;
    }

    active_node = picked_node;

    return picked_node;
}

void wait_for_processes(node *n) {
    remove_from_scheduler(n);

    /* insert the node into the zombie queue */
    pcb_t *process = (pcb_t *)n->payload;
    pcb_t *parent_process = process->parent;
    add_node(parent_process->zombies, n);
    
    if (!process->waited) {
        log_events(ZOMBIE, ticks, process->pid, process->priority, process->process);
    }

    // TODO: clean zombies & children
}

void schedule() {
    /* 1. do we want to keep the scheduler running? */
    // NO called by alarm handler
    /* 2. do we need to create a process for the scheduler? */
    // NO
    /* 3. mainContext & schedulerContext & shellContext in log example */

    /* check for any sleep nodes before we pick a node */
    node *cur = queue_sleep->head;

    while (cur) {
        pcb_t *cur_p = (pcb_t *) cur->payload;
        /* if the current sleep process is not finished */
        if (cur_p->ticks > 0) {
            if (cur_p->status != STOPPED_P) {
                cur_p->ticks--;
            }
        } else { /* finished sleep process */
            if (cur_p->status != EXITED_P) {
                cur_p->status = EXITED_P;
            }
            log_events(EXITED, ticks, cur_p->pid, cur_p->priority, cur_p->process);
            remove_node(queue_sleep, cur);
            k_unblock(cur_p->parent->pid);
            wait_for_processes(cur);
        }

        cur = cur->next;
    }

    node *next_process = pick_next_process();

    if (next_process == NULL) {
        setcontext(&idle_context);
        perror("setcontext - idle");
        exit(EXIT_FAILURE);
    }

    pcb_t *process = (pcb_t *) (next_process->payload);

    log_events(SCHEDULE, ticks, process->pid, process->priority, process->process);

    // setcontext process->context
    setcontext(&(process->context));

    // end of scheduler
    perror("setcontext");
    exit(EXIT_FAILURE);

    // exit the current process / insert the node back to the queue - some other functions
}