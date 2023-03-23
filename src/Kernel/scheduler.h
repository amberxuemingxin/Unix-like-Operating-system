#ifndef SCHEDULER_HEADER
#define SCHEDULER_HEADER

#include "queue.h"

/*Keeps all queues
 Ready queue: three priority queues 
 Zombie queue: one*/
 typedef struct scheduler_def
 {
    queue *queue_high;
    queue *queue_mid;
    queue *queue_low;
    queue *queue_zombie;
 } scheduler;


 // set up signals

 // pick a thread and run
 void schedule();

 #endif