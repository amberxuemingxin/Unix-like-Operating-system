#include "queue.h"

node *init_node(void *payload) {
    node *n = (node *)malloc(sizeof(node));
    n->payload = payload;
    n->next = NULL;

    return n;
}

queue *init_queue() {
    queue *q = (queue *)malloc(sizeof(queue));
    q->head = NULL;
    q->length = 0;

    return q;
}

void add_node(queue *q, node *n) {
    node *prev = q->head;

    while (prev && prev->next) {
        prev = prev->next;
    }

    if (prev == NULL) {
        q->head = n;
    } else {
        prev->next = n;
    }

    q->length++;

}

void free_node(node *n) {
    free(n->payload);
    free(n);
}

int remove_head(queue *q) {
    if (q->head == NULL) {
        return -1;
    }

    node *tmp = q->head;
    q->head = tmp->next;
    q->length--;

    free_node(tmp);
    return 0;
}

void free_queue(queue *q) {
    node *tmp;

    while (q->head) {
        tmp = q->head;
        q->head = tmp->next;
        free_node(tmp);
    }

    free(q);
}