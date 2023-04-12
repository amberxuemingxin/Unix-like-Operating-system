#include "queue.h"
#include "stdio.h" // debug purpose

queue *init_queue() {
    queue *q = (queue *)malloc(sizeof(queue));
    q->head = NULL;
    q->length = 0;

    return q;
}

void add_process(queue *q, pcb_t *p) {
    pcb_t *prev = q->head;
    while (prev && prev->next) {
        prev = prev->next;
    }

    if (prev == NULL) {
        q->head = p;
    } else {
        prev->next = p;
    }
    q->length++;
    // printf("process %s added, cur len = %d\n",p->process ,q->length);
}

pcb_t *remove_process(queue *q, pcb_t *p) {
    // printf("removed!\n");
    pcb_t *prev = NULL;
    pcb_t *tmp = q->head;

    while (tmp) {
        if (tmp == p) {
            if (prev) {
                prev->next = p->next;
            } else {
                q->head = p->next;
            }
            p->next = NULL;
            q->length--;
            // printf("process %s removed, cur len = %d\n",p->process ,q->length);
            return p;
        }
        prev = tmp;
        tmp = tmp->next;
    }
    return p;
}