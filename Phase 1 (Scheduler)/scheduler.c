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


struct msgbuff
{
    long mtype;
    char mtext[256];
};

int main(int argc, char *argv[])
{
    initClk();

    int algo_type = atoi(argv[0]);
    int quantum = atoi(argv[1]);
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    printf("schedular is started\n");

    key_t key_id;
    int rec_val, msgq_id;
    key_id = ftok("keyfile", 65);               //create unique key
    msgq_id = msgget(key_id, 0666 | IPC_CREAT); //create message queue and return id
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    printf("Message Queue ID = %d\n", msgq_id);

    struct msgbuff message;
    // The job of the scheduler
    while (1)
    {
        /* receive all types of messages */
        rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext), 0, IPC_NOWAIT);
        if (rec_val == -1)
        {
             continue;
        }
        else
        {
            ////if there is no new processes
            if (!strcmp(message.mtext,"Done !"))
            {
                printf("wohooo !\n");
                break;
            }
            else
            {
                printf("\nMessage received: %s with m-type: %ld \n", message.mtext, message.mtype);
            }
        }
    }
    destroyClk(true);
}
