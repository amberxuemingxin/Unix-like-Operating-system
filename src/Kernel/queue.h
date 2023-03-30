#ifndef QUEUE_HEADER
#define QUEUE_HEADER

#include <stdlib.h>
#include "PCB.h"

typedef struct LinkedList {
    pcb_t *head;
    int length;
} queue;

queue *init_queue();

void add_process(queue *q, pcb_t *p);

pcb_t *remove_process(queue *q, pcb_t *p);

void free_queue(queue *q);

#endif