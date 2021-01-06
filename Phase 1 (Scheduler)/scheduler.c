#include "headers.h"
/*
    how it should work
    1. Start a new process. (Fork it and give it its parameters.)
    2. Switch between two processes according to the scheduling algorithm. (Stop the
    old process and save its state and start/resume another one.)
    3. Keep a process control block (PCB) for each process in the system. A PCB
    should keep track of the state of a process; running/waiting, execution time,
    remaining time, waiting time, etc.
    4. Delete the data of a process when it gets notifies that it finished. When a
    process finishes it should notify the scheduler on termination, the scheduler
    does NOT terminate the process.
    5. Report the following information
        (a) CPU utilization.
        (b) Average weighted turnaround time.
        (c) Average waiting time.
        (d) Standard deviation for average weighted turnaround time.
    6. Generate two files: (check the input/output section below)
        (a) Scheduler.log
        (b) Scheduler.perf
    */
int main(int argc, char *argv[])
{
    initClk();

    //TODO implement the scheduler :)
    //upon termination release the clock resources.

    destroyClk(true);
}
