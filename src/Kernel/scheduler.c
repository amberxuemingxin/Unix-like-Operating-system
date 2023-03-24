#include <sys/time.h>  // setitimer

#include "scheduler.h"
#include "logger.h"
#include "PCB.h"

extern int ticks;

scheduler *init_scheduler() {
    scheduler *s = (scheduler *)malloc(sizeof(scheduler));

    s->ticks = 0;
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

void schedule() {
    // log_events(SCHEDULE, ticks, );
}