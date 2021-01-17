#include "headers.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#define MAX_PROCESSES 32000 // this is the maximum number of prcessess that \
                            // the system can handle
struct Process
{
    int id;
    int arrival_time;
    bool arrived;
    int runtime;
    int priority; // this is process priority ranges from 0 to 10 where 0 is the heighest priority
    int mem_size;
};
bool finished = false;
int msgq_id;
void clearResources(int signum);

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    FILE *pFile;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    pFile = fopen("processes.txt", "r");
    printf("errno %d", errno);
    struct Process sys_prcesses[MAX_PROCESSES]; // this is the data structure for storing the prcesses
    int curr_number_of_processes = 0;
    if (pFile == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, pFile)) != -1)
    {

        if (line[0] == '#') // ignores the lines starting with #
        {
            continue;
        }
        // printf("Retrieved line of length %zu:\n", read);
        int init_size = strlen(line);
        char delim[] = "\t";

        // read the id
        char *ptr = strtok(line, delim);
        sys_prcesses[curr_number_of_processes].id = atoi(ptr);

        // read the arrival
        ptr = strtok(NULL, delim);
        sys_prcesses[curr_number_of_processes].arrival_time = atoi(ptr);

        // read the runtime
        ptr = strtok(NULL, delim);
        sys_prcesses[curr_number_of_processes].runtime = atoi(ptr);

        // read the priority
        ptr = strtok(NULL, delim);
        sys_prcesses[curr_number_of_processes].priority = atoi(ptr);

        // read the memsize
        ptr = strtok(NULL, delim);
        sys_prcesses[curr_number_of_processes].mem_size = atoi(ptr);

        curr_number_of_processes++;
    }

    fclose(pFile);

    // now the sys_prcesses array contain
    // all the processes in ascending order of arrival time

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    printf("choose the sheduling algo you want: 0) HPF 1)SRTN 2)RR : \n");
    printf("--> ");
    int algo_type = HPF, quantum = -1;
    scanf("%d", &algo_type);

    if (algo_type == HPF)
    {
        printf("algorithm is  HPF\n");
    }
    else if (algo_type == SRTN)
    {
        printf("algorithm is  SRTN\n");
    }

    else
    {
        printf("algorithm is RR round robin\n");
        printf("enter Quantum duration: \n");
        printf("--> ");
        scanf("%d", &quantum);
    }

    // 3. Initiate and create the scheduler and clock processes.
    // a) start the scheduler as it will wait for the clk to wake ups
    if (fork() == 0)
    {
        char s_algo_type[20];
        char s_quantum[20];
        // convert 123 to string [buf]
        // itoa(algo_type, s_algo_type, 10);
        sprintf(s_algo_type, "%d", algo_type);
        sprintf(s_quantum, "%d", quantum);

        execl("./scheduler.out", s_algo_type, s_quantum);
        // execl("./scheduler.out", "", "");
        printf("error is %d\n", errno);
        printf("error occurred couldn't execute scheduler.out");
        exit(-1);
    }

    // b) start the clock
    if (fork() == 0)
    {

        execl("./clk.out", "");
        printf("error occurred couldn't execute clk.out\n");
    }

    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.

    //Getting PID and preparing message to message queue
    key_t key_id;
    int send_val;
    key_id = ftok("keyfile", 65);               //create unique key
    msgq_id = msgget(key_id, 0666 | IPC_CREAT); //create message queue and return id
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    printf("Message Queue ID = %d\n", msgq_id);

    //message to send
    char str[256];

    struct msgbuff message;
    long iD = getpid();

    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    int i = 0;
    while (1)
    {
        //get its arrival time
        if (sys_prcesses[i].arrival_time == getClk())
        {
            snprintf(str, sizeof(str), "%d %d %d %d %d", sys_prcesses[i].id, sys_prcesses[i].arrival_time, sys_prcesses[i].runtime, sys_prcesses[i].priority, sys_prcesses[i].mem_size);
            message.mtype = iD;
            strcpy(message.mtext, str);
            //printf("Genereator mess send clk is %d \n",getClk());
            send_val = msgsnd(msgq_id, &message, sizeof(message.mtext), IPC_NOWAIT);

            if (send_val == -1)
                perror("Error in send");
            curr_number_of_processes--;
            i++;
        }
        // else
        // {
        //     strcpy(message.mtext, "pass");
        //     //printf("Genereator mess send clk is %d \n",getClk());
        //     message.mtype = iD;
        //     send_val = msgsnd(msgq_id, &message, sizeof(message.mtext), IPC_NOWAIT);
        // }
        //if all processes are served
        if (curr_number_of_processes == 0 && !finished)
        {
            strcpy(message.mtext, "Done !");
            send_val = msgsnd(msgq_id, &message, sizeof(message.mtext), IPC_NOWAIT);
            finished = true;
        }
    }
}

void clearResources(int signum)
{

    //TODO Clears all resources in case of interruption
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0);
    destroyClk(true); // changed it to false so that the schedular is the one
    exit(0);
}
