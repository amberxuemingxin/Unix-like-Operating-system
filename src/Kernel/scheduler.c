#include <sys/time.h>  // setitimer
#include <stdlib.h> // rand
#include <signal.h> // sigaction, sigemptyset, sigfillset, signal

#include "scheduler.h"
#include "logger.h"
#include "PCB.h"

ucontext_t *active_context;

extern ucontext_t scheduler_context;
extern int ticks;

scheduler *init_scheduler() {
    scheduler *s = (scheduler *)malloc(sizeof(scheduler));

    s->queue_high = init_queue();
    // printf("init high: %d\n", s->queue_high->length);
    s->queue_mid = init_queue();
    // printf("init mid: %d\n", s->queue_mid->length);
    s->queue_low = init_queue();
    // printf("init low: %d\n", s->queue_low->length);
    s->queue_zombie = init_queue();

    return s;
}

void alarm_handler(int signum) {
    swapcontext(active_context, &scheduler_context);
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

    printf("low: %d, mid: %d, high: %d\n", s->queue_low->length, s->queue_mid->length, s->queue_high->length);

    int low_length = s->queue_low->length;
    int mid_length = (int) (s->queue_mid->length * 1.5);
    int high_length = (int) (s->queue_high->length * 1.5 * 1.5);

    printf("low: %d, mid: %d, high: %d\n", low_length, mid_length, high_length);

    bool low_queue_existed = (low_length > 0) ? true : false;
    bool mid_queue_existed = (mid_length > 0) ? true : false;

    int picked_queue = rand() % (low_length + mid_length + high_length);
    
    if (picked_queue < low_length && low_queue_existed) {
        picked_node = s->queue_low->head;
    } else if (picked_queue < low_length + mid_length && mid_queue_existed) {
        picked_node = s->queue_mid->head;
    } else {
        picked_node = s->queue_high->head;
    }

    return picked_node;
}

void schedule(scheduler *s) {
    /* 1. do we want to keep the scheduler running? */
    // NO called by alarm handler
    /* 2. do we need to create a process for the scheduler? */
    // NO
    /* 3. mainContext & schedulerContext & shellContext in log example */
    node *next_process = pick_next_process(s);
    pcb_t *process = (pcb_t *) (next_process->payload);

    log_events(SCHEDULE, ticks, process->pid, process->priority, process->process);

    // setcontext process->context
    setcontext(&(process->context));

    // end of scheduler
    perror("setcontext");
    exit(EXIT_FAILURE);

    // exit the current process / insert the node back to the queue - some other functions
}