#include <sys/time.h>  // setitimer
#include <stdlib.h> // rand
#include <signal.h> // sigaction, sigemptyset, sigfillset, signal

#include "scheduler.h"
#include "kernel.h"
#include "logger.h"
#include "PCB.h"

pcb_t *active_process;

queue *queue_high;
queue *queue_mid;
queue *queue_low;

extern bool idle;
extern ucontext_t scheduler_context;
extern ucontext_t idle_context;
extern int ticks;

// TODO: remove sleep queue
void init_scheduler() {
    queue_high = init_queue();
    queue_mid = init_queue();
    queue_low = init_queue();

    active_process = NULL;
}

void alarm_handler(int signum) {
    if (signum == SIGALRM) {
        ticks++;
    }
    if (idle) {
        setcontext(&scheduler_context);
    } else {
        // printf("current active process: %s\n", active_process->process);
        swapcontext(&(active_process->context), &scheduler_context);
    }
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

void add_to_scheduler(pcb_t *process) {
    
    if (process->priority == -1) {
        add_process(queue_high, process);
    } else if (process->priority == 0) {
        add_process(queue_mid, process);
    } else if (process->priority == 1) {
        add_process(queue_low, process);
    }
}

void remove_from_scheduler(pcb_t *process) {

    if (process->priority == -1) {
        remove_process(queue_high, process);
    } else if (process->priority == 0) {
        remove_process(queue_mid, process);
    } else {
        remove_process(queue_low, process);
    }
}

pcb_t *search_in_scheduler(pid_t pid) {
    pcb_t *tmp = queue_high->head;

    while (tmp) {
        if (tmp->pid == pid) {
            return tmp;
        }
        tmp = tmp->next;
    }

    tmp = queue_mid->head;
    while (tmp) {
        if (tmp->pid == pid) {
            return tmp;
        }

        tmp = tmp->next;
    }
    
    tmp = queue_low->head;
    while (tmp) {
        if (tmp->pid == pid) {
            return tmp;
        }

        tmp = tmp->next;
    }

    return NULL;
}

pcb_t *pick_next_process() {
    pcb_t *picked_process;

    bool low_queue_existed = (queue_low->length > 0) ? true : false;
    bool mid_queue_existed = (queue_mid->length > 0) ? true : false;
    bool high_queue_existed = (queue_high->length > 0) ? true : false;

    if (!low_queue_existed && !mid_queue_existed && !high_queue_existed) {
        idle = true;
        return NULL;
    }

    idle = false;
    int low_length = queue_low->length;
    int mid_length = (int) (queue_mid->length * 1.5);
    int high_length = (int) (queue_high->length * 1.5 * 1.5);

    int picked_queue = rand() % (low_length + mid_length + high_length);
    
    if (picked_queue < low_length && low_queue_existed) {
        picked_process = queue_low->head;
    } else if (picked_queue < low_length + mid_length && mid_queue_existed) {
        picked_process = queue_mid->head;
    } else {
        picked_process = queue_high->head;
        // printf("picked high!\n");
    }

    return picked_process;
}

void wait_for_processes(pcb_t *p) {
    remove_from_scheduler(p);

    /* insert the node into the zombie queue */
    pcb_t *parent = p->parent;
    
    pcb_t *zombie = parent->zombies;
    while (zombie) {
        zombie = zombie->next;
    }

    if (zombie != NULL) {
        zombie->next = p;
    } else {
        parent->zombies = p;
    }
    
    if (!p->waited) {
        log_events(ZOMBIE, ticks, p->pid, p->priority, p->process);
    }

    // TODO: clean zombies & children
}

void schedule() {
/*
* Check the sleeping nodes here!
*/
    if (active_process) {
        // perror("here");
        if (active_process->ticks > 0) {
            // printf("ticks %d\n", active_process->ticks);
            active_process->ticks--;
            setcontext(&idle_context);
        } else if (active_process->ticks == 0) {
            k_unblock(active_process->parent);
            remove_from_scheduler(active_process);
            // printf("exit %s\n", active_process->process);
            log_events(EXITED, ticks, active_process->pid, active_process->priority, active_process->process);
            free_pcb(active_process);
            active_process = NULL;
        }
    }

    pcb_t *next_process = pick_next_process();

    if (next_process == NULL) {
        // printf("idle process picked!\n");
        setcontext(&idle_context);
        perror("setcontext - idle");
        exit(EXIT_FAILURE);
    }

    if (active_process != next_process) {
        active_process = next_process;
        log_events(SCHEDULE, ticks, active_process->pid, active_process->priority, active_process->process);
    }

    // setcontext process->context
    setcontext(&(active_process->context));

    // end of scheduler
    perror("setcontext");
    exit(EXIT_FAILURE);

    // exit the current process / insert the node back to the queue - some other functions
}