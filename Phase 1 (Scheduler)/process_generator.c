#include "headers.h"

struct Process
{
    int id;
    int arrival;
    int runtime;
    int priority; // this is process priority ranges from 0 to 10 where 0 is the heighest priority
};
void clearResources(int);

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    FILE *pFile;
    pFile = fopen("processes.txt", "r");
    char buff[255];

    fscanf(pFile, "%s", buff);
    printf("1 : %s\n", buff);

    fclose(pFile);

    return 0;
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    // 3. Initiate and create the scheduler and clock processes.
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}
