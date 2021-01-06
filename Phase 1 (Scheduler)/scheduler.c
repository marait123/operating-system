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

int msg_q_id;

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
    int msg_q_key = ftok("keyfile", 65);
    msg_q_id = msgget(msg_q_key, 0666 | IPC_CREAT);

    struct msgbuff message;
    while (1)
    {
        /* receive all types of messages */
        // rec_val = msgrcv(up_q_id, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
        // if (rec_val == -1)
        // {
        //     perror("Error in receive");
        //     exit(-1);
        // }
        // else
        // {
        //     printf("\nMessage received: %s with m-type: %d \n", message.mtext, message.mtype);
        //     // convert the message to upper
        //     int len = strlen(message.mtext);
        //     conv(message.mtext, len);
        //     send_val = msgsnd(down_q_id, &message, sizeof(message.mtext), !IPC_NOWAIT);
        // }
    }
    destroyClk(true);
}
