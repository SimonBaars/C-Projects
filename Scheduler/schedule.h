#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <assert.h>

typedef enum {
    NEW_PROCESS_EVENT,
    TIME_EVENT,
    READY_EVENT,
    IO_EVENT,
    FINISH_EVENT
} event_type;

typedef struct student_pcb {
    void *sim_pcb;
    int priority;
    struct student_pcb *prev, *next;
    long mem_need, mem_base;
} student_pcb;

// The functions below are convenient to use for process queue manipulation.
// Feel free to implement more of your own
// functions if these functions do not do what you want.

// Returns the length of a queue
static int queue_length(student_pcb **queue) {
    int length = 0;

    student_pcb *current = *queue;
    while (current) {
        ++length;
        current = current->next;
    }

    return length;
}

// Attaches a new item at the front of a queue (item must not be in any queue,
// i.e. newly created or removed)
static void queue_prepend(student_pcb **queue, student_pcb *item) {
    assert(item->prev == NULL);
    assert(item->next == NULL);

    item->next = *queue;
    if (*queue) {
        (*queue)->prev = item;
    }
    *queue = item;
    item->prev = NULL;
}

// Retrieves the last item of a queue
static student_pcb *queue_last(student_pcb **queue) {
    student_pcb *current = *queue;
    student_pcb *last = NULL;

    while (current) {
        last = current;
        current = current->next;
    }

    return last;
}

// Removes an item from a queue (THIS FUNCTION DOES NOT CHECK WHETHER THE ITEM
// IS IN THE QUEUE AND WILL NOT BEHAVE PROPERLY WHEN THE WRONG QUEUE IS PASSED)
static void queue_remove(student_pcb **queue, student_pcb *item) {
    student_pcb *next = item->next;

    // Fix the next pointer of the node before this (or the head of the list if
    // there is no previous)
    if (item == *queue) {
        assert(item->prev == NULL);
        *queue = next;
    } else {
        item->prev->next = next;
    }

    // Fix the previous pointer of the node after this
    if (item->next) {
        next->prev = item->prev;
    }

    item->next = NULL;
    item->prev = NULL;
}

// Append an item at the back of the queue (item must not be in any queue, i.e.
// newly created or removed)
static void queue_append(student_pcb **queue, student_pcb *item) {
    assert(item->next == NULL);
    assert(item->prev == NULL);

    if (*queue) {
        // Queue is non-empty, find the last node and attach item to it
        student_pcb *last = queue_last(queue);
        assert(last->next == NULL);
        last->next = item;
        item->prev = last;
    } else {
        // Queue is empty
        *queue = item;
    }
}

student_pcb *new_proc, *ready_proc, *io_proc, *defunct_proc;

double sim_time();

extern void set_slice(double slice);

long rm_process(student_pcb **proces);

typedef void function();

function *finale;

function *reset_stats;

/****************************************************************************
   De door de practicanten te schrijven routine
****************************************************************************e */

void schedule(event_type event, int nTry, int nProcs);

void addProcToQueue(student_pcb **appropriateQueue, student_pcb *proc);

#endif /* SCHEDULE_H */