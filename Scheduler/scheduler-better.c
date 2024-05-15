#include <stdio.h>
#include <stdlib.h>

#include "schedule.h"
#include "mem_alloc.h"

/* This file contains a bare bones skeleton for the scheduler function
   for the second assignment for the OSN course of the 2005 fall
   semester.
   Author: Simon Baars

   Date:
   October 23, 2003
   Modified:
   April 29, 2018
 */

/* This variable will simulate the allocatable memory */

static long memory[MEM_SIZE];

double INITIAL_SLICE_SIZE = 100;
double PRIORITY_MULTIPLIER = 2;

/* The actual CPU scheduler is implemented here */

static void cpu_scheduler(int nProcs) {
    if (ready_proc) {
        set_slice((INITIAL_SLICE_SIZE * ((ready_proc->priority * PRIORITY_MULTIPLIER) + 1)) / nProcs);
        ready_proc->priority++;
        addProcToQueue(&ready_proc, ready_proc);
    }
}

/* The high-level memory allocation scheduler is implemented here */
static void give_memory(int nTry) {
    int index;
    int i;
    for (i = 0; new_proc != NULL && i <= nTry; i++) {
        student_pcb *proc1 = new_proc, *proc2 = new_proc;

        for (; proc1 != NULL; proc1 = proc1->next) {
            if (proc1->mem_need < proc2->mem_need)
                proc2 = proc1;
        }

        index = mem_get(proc2->mem_need);

        if (index >= 0) {
            proc2->mem_base = index;
            addProcToQueue(&new_proc, proc2);
            return;
        }
    }
}

void addProcToQueue(student_pcb **appropriateQueue, student_pcb *proc) {
    queue_remove(appropriateQueue, proc);
    queue_append(&ready_proc, proc);
}

/* Here we reclaim the memory of a process after it
  has finished */

static void reclaim_memory() {
    student_pcb *proc;

    proc = defunct_proc;
    while (proc) {
        /* Free the simulated allocated memory
         */
        mem_free(proc->mem_base);
        proc->mem_base = -1;

        /* Call the function that cleans up the simulated process
         */
        rm_process(&proc);

        /* See if there are more processes to be removed
         */
        proc = defunct_proc;
    }
}

/* You may want to have the last word... */

static void my_finale() {
    /* Your very own code goes here */
}

/* The main scheduling routine */

void schedule(event_type event, int nTry, int nProcs) {
    static int first = 1;

    if (first) {
        mem_init(memory);
        finale = my_finale;
        first = 0;

        printf("==========================================================================\n"
                "==              Welcome to the better scheduling algorithm.             ==\n"
                "== The better algorithm consists of the RoundRobin (simple) algorithm   ==\n"
                "== combined with a fairness threshold. Processes with a higher fairness ==\n"
                "==                    get bigger CPU slices.                            ==\n"
                "==========================================================================\n");
    }

    switch (event) {
        case NEW_PROCESS_EVENT:
            give_memory(nTry);
            break;
        case TIME_EVENT:
        case IO_EVENT:
            cpu_scheduler(nProcs);
            break;
        case READY_EVENT:
            break;
        case FINISH_EVENT:
            reclaim_memory();
            give_memory(nTry);
            cpu_scheduler(nProcs);
            break;
        default:
            printf("I cannot handle event nr. %d\n", event);
            break;
    }
}
