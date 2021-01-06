#include "headers.h"
#include <string.h>

#define MAX_PROCESSES 32000 // this is the maximum number of prcessess that \
                            // the system can handle
struct Process
{
    int id;
    int arrival_time;
    bool arrived;
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
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    pFile = fopen("processes.txt", "r");
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

        curr_number_of_processes++;
        // #id arrival runtime priority
        // while (ptr != NULL)
        // {
        //     printf("'%s'\n", ptr);
        //ptr = strtok(NULL, delim);
        // }

        // printf("%s", line);
    }

    fclose(pFile);

    // now the sys_prcesses array contain
    // all the processes in ascending order of arrival time

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.

    return 0;

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
