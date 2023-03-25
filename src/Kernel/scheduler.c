#include <sys/time.h>  // setitimer
#include <stdlib.h> // rand

#include "scheduler.h"
#include "logger.h"
#include "PCB.h"

extern int ticks;

scheduler *init_scheduler() {
    scheduler *s = (scheduler *)malloc(sizeof(scheduler));

    s->queue_high = init_queue();
    s->queue_mid = init_queue();
    s->queue_low = init_queue();
    s->queue_zombie = init_queue();

    return s;
}

void set_timer() {
    struct itimerval it;

    it.it_interval = (struct timeval) { .tv_usec = CENTISECOND * 10 };
    it.it_value = it.it_interval;

    setitimer(ITIMER_REAL, &it, NULL);
}

void add_to_scheduler(node *n, scheduler *s) {
    pcb_t *process = (pcb_t *)n->payload;
    
    if (process->priority == -1) {
        add_node(s->queue_high, n);
    } else if (process->priority == 0) {
        add_node(s->queue_mid, n);
    } else if (process->priority == 1) {
        add_node(s->queue_low, n);
    }
}

void remove_from_scheduler(node *n, scheduler *s) {
    pcb_t *process = (pcb_t *)n->payload;

    if (process->priority == -1) {
        remove_node(s->queue_high, n);
    } else if (process->priority == 0) {
        remove_node(s->queue_mid, n);
    } else if (process->priority == 1) {
        remove_node(s->queue_low, n);
    }
}

node *pick_next_process(scheduler *s) {
    node *picked_node;

    int low_length = s->queue_low->length;
    int mid_length = (int) (s->queue_mid->length * 1.5);
    int high_length = (int) (s->queue_high->length * 1.5 * 1.5);

    bool low_queue_existed = (low_length > 0) ? true : false;
    bool mid_queue_existed = (mid_length > 0) ? true : false;
    bool high_queue_existed = (high_length > 0) ? true : false;

    int picked_queue = rand() % (low_length + mid_length + high_length);
    
    if (picked_queue < low_length && low_queue_existed) {
        picked_node = s->queue_low->head;
    } else if (picked_queue < low_length + mid_length && mid_queue_existed) {
        picked_node = s->queue_mid->head;
    } else if (picked_queue < low_length + mid_length + high_length && high_queue_existed) {
        picked_node = s->queue_high->head;
    }

    return picked_node;
}

void schedule(scheduler *s) {
    while (1) {
        node *next_process = pick_next_process(s);
        pcb_t *process = (pcb_t *) (next_process->payload);

        log_events(SCHEDULE, ticks, process->pid, process->priority, process->process);

        // switch user context

        // execute the process

        // exit the current process / insert the node back to the queue

    }
}