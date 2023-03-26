#ifndef SCHEDULER_HEADER
#define SCHEDULER_HEADER

#include "queue.h"

#define CENTISECOND 10000 /* 10 millisconds */

/*Keeps all queues
 Ready queue: three
 Zombie queue: one*/

void init_scheduler();

void set_timer();

void set_alarm_handler();

void wait_for_processes(node *n);

void schedule();

void add_to_scheduler(node *n);

void remove_from_scheduler(node *n);

 #endif