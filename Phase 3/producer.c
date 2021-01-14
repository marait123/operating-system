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
#define MSG_SIZE 4096

// the code handles any number of clients simultaneously

int up_q_id = 0, down_q_id = 1, shm_id = 2;
struct msgbuff
{
    long mtype;
    char mtext[MSG_SIZE];
};

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

        strcpy((char *)shmaddr, msg);
        // strcpy(shmaddr, "quit");
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

    // printf("\reader Detaching...\n");
    shmdt(shmaddr);
}

int main()
{

    int up_q_key = ftok("keyfile", 65);
    int down_q_key = ftok("keyfile", 66);
    int shm_key = ftok("keyfile", 67);
    up_q_id = msgget(up_q_key, 0666 | IPC_CREAT);
    down_q_id = msgget(down_q_key, 0666 | IPC_CREAT);
    shm_id = shmget(shm_key, 4096, IPC_CREAT | 0644);
    if (up_q_id == -1 || down_q_id == -1 || shm_id == -1)
    {
        perror("Error in create\n");
        exit(-1);
    }
    printf("up Queue ID = %d\n", up_q_id);
    printf("down Queue ID = %d\n", down_q_id);
    printf("shared memory ID = %d\n", shm_id);

    while (1)
    {

        // 1. read the message from terminal
        printf("Please enter a line:\n");
        char *message_text = NULL;
        size_t len = 0;
        ssize_t lineSize = 0;
        lineSize = getline(&message_text, &len, stdin);

        char message_to_write[MSG_SIZE];
        strcpy(message_to_write, message_text);
        // 2. write the message text in memory
        msg_write(message_to_write);

        // 3. send a message on the up queue
        struct msgbuff message;
        long mtype = getpid();
        message.mtype = mtype;
        printf("process id %d\n", mtype);

        strcpy(message.mtext, "start conversion");

        int send_val = msgsnd(up_q_id, &message, sizeof(message.mtext), !IPC_NOWAIT);
        if (send_val == -1)
            perror("Errror in send");

        // 3. wait for the message on the down queue
        // withe the same m-type
        int rec_val = msgrcv(down_q_id, &message, sizeof(message.mtext), mtype, !IPC_NOWAIT);

        if (rec_val == -1)
            perror("Error in receive");
        else
        {
            // read the message from shared memory
            msg_read(message_text);
            printf("the upper case: %s.\n", message_text);
            printf("\nMessage received: %s.\n", message.mtext);
        }
        free(message_text);
    }

    return 0;
}
