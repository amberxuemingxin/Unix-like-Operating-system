#ifndef SCHEDULER_HEADER
#define SCHEDULER_HEADER

#include "queue.h"

#define CENTISECOND 10000 /* 10 millisconds */

/*Keeps all queues
 Ready queue: three
 Zombie queue: one*/
 typedef struct scheduler_def
 {
    queue *queue_high;
    queue *queue_mid;
    queue *queue_low;
    queue *queue_zombie;
 } scheduler;

scheduler *init_scheduler();

void set_timer();

void set_alarm_handler();

void schedule(scheduler *s);

void add_to_scheduler(node *n, scheduler *s);

void remove_from_scheduler(node *n, scheduler *s);

 #endif