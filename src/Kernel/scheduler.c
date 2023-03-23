#include "scheduler.h"

scheduler *init_scheduler() {
    scheduler *s = (scheduler *)malloc(sizeof(scheduler));

    s->ticks = 0;
    s->queue_high = init_queue();
    s->queue_mid = init_queue();
    s->queue_low = init_queue();
    s->queue_zombie = init_queue();

    return s;
}