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

int main()
{
    char cc[100];

    int *address = &cc;

    *address = 1500;
    for (int i = 0; i < 5; i++)
    {
        *address = (i + 1) * 2;
        address++;
    }
    address = &cc;
    for (int i = 0; i < 5; i++)
    {
        printf("%d \n", *address);
        *address++;
    }

    return 0;
}