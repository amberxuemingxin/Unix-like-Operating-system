#include "PCB.h"

typedef struct ListNode {
    pcb_t *context;
    node *next;
} node; 

typedef struct LinkedList {
    node *head;
    int length;
} queue;

node *init_node(pcb_t *new_pcb);

queue *init_queue();

int add_node(queue *q, node *node);

int remove_node(queue *q, node *remove);

void free_node(node *node);

void free_queue(queue *q);