#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/shm.h>
#include <signal.h>
#define MSG_SIZE 4096
// the code handles any number of clients simultaneously

struct msgbuff
{
    long mtype;
    char mtext[MSG_SIZE];
};
int up_q_id = 0;
int down_q_id = 0;
int shm_id = 0;

void exit_handler(int sigId)
{
    msgctl(up_q_id, IPC_RMID, (struct msqid_ds *)0);
    msgctl(down_q_id, IPC_RMID, (struct msqid_ds *)0);
    shmctl(shm_id, IPC_RMID, (struct shmid_ds *)0);
    exit(0);
}

void conv(char *msg, int size)
{
    int i;
    for (i = 0; i < size; ++i)
        if (islower(msg[i]))
            msg[i] = toupper(msg[i]);
        else if (isupper(msg[i]))
            msg[i] = tolower(msg[i]);
}

void msg_write(char *msg)
{
    // printf("memory id to write %d", shm_id);
    void *shmaddr = shmat(shm_id, (void *)0, 0);
    if (shmaddr == -1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }
    else
    {
        // printf("\nWriter: Shared memory attached at address %x\n", shmaddr);
        // printf("message to send: %s", msg);

        // strcpy((char *)shmaddr, "quit");
        strcpy(shmaddr, msg);
    }

    // printf("\nWriter Detaching...\n");
    shmdt(shmaddr);
}

void msg_read(char *msg)
{
    void *shmaddr = shmat(shm_id, (void *)0, 0);
    if (shmaddr == -1)
    {
        perror("Error in attach in reader");
        exit(-1);
    }
    else
    {
        // printf("\reader: Shared memory attached at address %x\n", shmaddr);
        strcpy(msg, (char *)shmaddr);
    }

    // printf("\nreader Detaching...\n");
    shmdt(shmaddr);
}

int main()
{
    key_t key_id;
    int rec_val;
    int send_val;
    signal(SIGINT, exit_handler);
    int up_q_key = ftok("keyfile", 65);
    int down_q_key = ftok("keyfile", 66);
    int shm_key = ftok("keyfile", 67);

    up_q_id = msgget(up_q_key, 0666 | IPC_CREAT);
    down_q_id = msgget(down_q_key, 0666 | IPC_CREAT);
    shm_id = shmget(shm_key, 4096, IPC_CREAT | 0666);

    if (up_q_id == -1 || down_q_id == -1 || shm_id == -1)
    {
        perror("Error in create\n");
        exit(-1);
    }
    printf("up Queue ID = %d\n", up_q_id);
    printf("down Queue ID = %d\n", down_q_id);
    printf("shared memory ID = %d\n", shm_id);

    // initialize the memory

    struct msgbuff message;
    while (1)
    {
        /* receive all types of messages */
        rec_val = msgrcv(up_q_id, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);

        if (rec_val == -1)
        {
            perror("Error in receive");
            exit(-1);
        }
        else
        {
            printf("\nMessage received: %s with m-type: %d \n", message.mtext, message.mtype);
            // convert the message to upper

            char messageText[MSG_SIZE];

            // read the message from the shared memory
            msg_read(messageText);
            int len = strlen(messageText);
            // convert it
            conv(messageText, len);

            // conv(message.mtext, len);
            // write the message to shared memory
            msg_write(messageText);

            // send done message
            strcpy(message.mtext, "finished conversion");
            send_val = msgsnd(down_q_id, &message, sizeof(message.mtext), !IPC_NOWAIT);
        }
    }

    return 0;
}
