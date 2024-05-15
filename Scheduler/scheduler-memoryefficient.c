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
   September 29, 2005
 */

/* This variable will simulate the allocatable memory */

static long memory[MEM_SIZE];

double INITIAL_SLICE_SIZE = 9.9e12;

/* The actual CPU scheduler is implemented here */

static void cpu_scheduler() {
    if (ready_proc) {
        set_slice(INITIAL_SLICE_SIZE);

        student_pcb *proc1 = ready_proc, *proc2 = ready_proc;
        for (; proc1 != NULL; proc1 = proc1->next) {
            if (proc1->mem_need > proc2->mem_need)
                proc2 = proc1;
        }
        addProcToQueue(&ready_proc, proc2);
    }
}

/* The high-level memory allocation scheduler is implemented here */

static void give_memory(int nTry) {
    int index;
    student_pcb *proc1 = new_proc;
    int i;
    for (i = 0; proc1 != NULL && i <= nTry; i++) {
        /* Search for a new process that should be given memory.
           Insert search code and criteria here.
           Attempt to allocate as follows:
         */

        index = mem_get(proc1->mem_need);


        if (index >= 0) {
            /* Allocation succeeded, now put in administration
             */
            proc1->mem_base = index;

            addProcToQueue(&new_proc, proc1);
            break;
        }
        proc1 = new_proc;
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

        printf("===============================================================================\n"
                "==           Welcome to the Memory Efficient scheduling algorithm.           ==\n"
                "== The Memory Efficient scheduling algorithm is based on Smallest Job First. ==\n"
                "===============================================================================\n");
        printf("We are currently scheduling %d processes, each given a timeslice of %f\n", nProcs, INITIAL_SLICE_SIZE);
    }

    switch (event) {
        case NEW_PROCESS_EVENT:
            give_memory(nTry);
            break;
        case TIME_EVENT:
        case IO_EVENT:
            cpu_scheduler();
            break;
        case READY_EVENT:
            break;
        case FINISH_EVENT:
            reclaim_memory();
            give_memory(nTry);
            cpu_scheduler();
            break;
        default:
            printf("I cannot handle event nr. %d\n", event);
            break;
    }
}
