#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/sem.h>
#define MSG_SIZE 4096
#define BUFF_SIZE 15
// the code handles any number of clients simultaneously
// 1. problem in removing
struct msgbuff
{
    long mtype;
    char mtext[MSG_SIZE];
};
int up_q_id = 0;
int down_q_id = 0;
// int shm_id = 0;
int BUFF_ID, BUFF_START_ID, BUFF_END_ID, BUFF_FULL_ID;

int *BUFF_START_ADRS;
int *BUFF_END_ADRS;
int *BUFF_ADRS;
int *BUFF_FULL_ADRS;
int SEM_ID;
void exit_handler(int sigId)
{
    //TODO: BUFF_FULL_ID check that is deleted
    //TODO: check there is no reference before deletion
    // printf("exit BUFF_FULL_ID %d", BUFF_FULL_ID);
    shmdt(BUFF_START_ADRS);
    shmdt(BUFF_END_ADRS);
    shmdt(BUFF_ADRS);
    shmdt(BUFF_FULL_ADRS);
    msgctl(up_q_id, IPC_RMID, (struct msqid_ds *)0);
    msgctl(down_q_id, IPC_RMID, (struct msqid_ds *)0);
    shmctl(BUFF_ID, IPC_RMID, (struct shmid_ds *)0);
    shmctl(BUFF_END_ID, IPC_RMID, (struct shmid_ds *)0);
    shmctl(BUFF_START_ID, IPC_RMID, (struct shmid_ds *)0);
    semctl(BUFF_FULL_ID, IPC_RMID, (struct shmid_ds *)0);
    semctl(SEM_ID, IPC_RMID, (struct semid_ds *)0);
    exit(0);
}

/* arg for semctl system calls. */
union Semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    ushort *array;         /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};

void down(int sem)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &p_op, 1) == -1)
    {
        perror("Error in down()");
        exit(-1);
    }
}

void up(int sem)
{
    struct sembuf v_op;
    v_op.sem_num = 0;
    v_op.sem_op = 1;
    v_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &v_op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}

int main()
{
    key_t key_id;
    int rec_val;
    int send_val;
    signal(SIGINT, exit_handler);
    int up_q_key = ftok("keyfile", 65);
    int down_q_key = ftok("keyfile", 66);
    int BUFF_key = ftok("keyfile", 67);
    int BUFF_START_key = ftok("keyfile", 68);
    int BUFF_END_key = ftok("keyfile", 69);
    int BUFF_FULL_key = ftok("keyfile", 70);
    int SEM_key = ftok("keyfile", 71);

    BUFF_START_ID = shmget(BUFF_START_key, sizeof(int), IPC_CREAT | 0666);
    BUFF_END_ID = shmget(BUFF_END_key, sizeof(int), IPC_CREAT | 0666);
    BUFF_FULL_ID = shmget(BUFF_FULL_key, sizeof(int), IPC_CREAT | 0666);
    // now initialize BUFF_START_ID BUFF_END_ID
    BUFF_START_ADRS = shmat(BUFF_START_ID, (void *)0, 0);
    BUFF_END_ADRS = shmat(BUFF_END_ID, (void *)0, 0);
    BUFF_FULL_ADRS = shmat(BUFF_FULL_ID, (void *)0, 0);
    SEM_ID = semget(SEM_key, 1, 0666 | IPC_CREAT);
    BUFF_ID = shmget(BUFF_key, BUFF_SIZE * sizeof(int), IPC_EXCL | IPC_CREAT | 0666);

    // this means it is not initialized yet
    if (BUFF_ID != -1)
    {
        BUFF_ID = shmget(BUFF_key, BUFF_SIZE * sizeof(int), IPC_CREAT | 0666);

        // initialize memory
        *BUFF_START_ADRS = 0;
        *BUFF_END_ADRS = 0;
        *BUFF_FULL_ADRS = 0;
        union Semun semun;
        // intialize semphore
        semun.val = 1; /* initial value of the semaphore, Binary semaphore */
        if (semctl(SEM_ID, 0, SETVAL, semun) == -1)
        {
            perror("Error in semctl");
            exit(-1);
        }
    }
    BUFF_ID = shmget(BUFF_key, BUFF_SIZE * sizeof(int), IPC_CREAT | 0666);

    BUFF_ADRS = shmat(BUFF_ID, (void *)0, 0);

    if (BUFF_START_ADRS == -1 || BUFF_END_ADRS == -1 || BUFF_ADRS == -1)
    {
        perror("Error in attach");
        exit(-1);
    }

    // debug
    // BUFF_START_ID
    // BUFF_END_ID
    // BUFF_FULL_ID
    printf("BUFF_START_ID %d \n", BUFF_START_ID);
    printf("BUFF_END_ID %d \n", BUFF_END_ID);
    printf("BUFF_FULL_ID %d \n", BUFF_FULL_ID);
    printf("BUFF_ID %d \n", BUFF_ID);
    //end
    up_q_id = msgget(up_q_key, 0666 | IPC_CREAT);
    down_q_id = msgget(down_q_key, 0666 | IPC_CREAT);
    if (up_q_id == -1 || down_q_id == -1)
    {
        perror("Error in creating up and down queues\n");
        exit(-1);
    }
    printf("up Queue ID = %d\n", up_q_id);
    printf("down Queue ID = %d\n", down_q_id);
    printf("BUFF_ID  = %d\n", BUFF_ID);

    // initialize the semaphore
    // TODO:
    int buff_number = 0;
    struct msgbuff message;
    // producer writes buff_number starting from the buff_end
    /*
    * If the buffer is empty, it waits for a message from the producer telling it that
    it has produced something.
    • If the buffer is full, it consumes an item then sends a message (using message
    passing) to the producer telling it that the buffer is no longer full.
    • If the buffer is neither empty nor full, it consumes an item from the buffer
    */
    // TODO: problem when producer is faster than consumer
    //TODO: check that more than one producer and cosumers are allowed
    while (1)
    {

        // check if buffer is empty
        down(SEM_ID);
        printf("semaphore in producer\n");
        // if buffer empty
        int count = *BUFF_FULL_ADRS;
        if (count == 0)
        {

            printf("buffer is empty\n");
            int *target_addr = BUFF_ADRS + *BUFF_END_ADRS;
            *target_addr = buff_number;
            buff_number++;
            (*BUFF_END_ADRS) = ((*BUFF_END_ADRS) + 1) % BUFF_SIZE;
            message.mtype = 0;
            strcpy(message.mtext, "not_empty");
            up(SEM_ID);
            send_val = msgsnd(up_q_id, &message, sizeof(message.mtext), !IPC_NOWAIT);
            down(SEM_ID);
            (*BUFF_FULL_ADRS)++; // put here for a reason (so that the message is consumed)
        }
        else if (count == BUFF_SIZE)
        {
            printf("buffer is full\n");
            message.mtype = 0;
            up(SEM_ID);
            rec_val = msgrcv(down_q_id, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
            down(SEM_ID);
            if (rec_val == -1)
            {
                perror("Error in receive\n");
                exit(-1);
            }
            printf("something was consumed\n");
            // buff_number++;
        }
        else
        {
            printf("buffer is neither empty nor full\n");
            int *target_addr = BUFF_ADRS + *BUFF_END_ADRS;
            *target_addr = buff_number;
            buff_number++;
            (*BUFF_END_ADRS) = ((*BUFF_END_ADRS) + 1) % BUFF_SIZE;
            (*BUFF_FULL_ADRS)++;
        }
        printf("number of items %d\n", count);
        printf("current buff_number %d\n", buff_number);
        sleep(4);
        up(SEM_ID);
    }
    return 0;
}
