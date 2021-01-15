#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    
    //TODO it needs to get the remaining time from somewhere
    remainingtime = atoi(argv[0]);
    while (remainingtime > 0)
    {
        remainingtime--;
        break;
        // Another Solution: wait the clock to be inc then iterate once more 
    }
    if(remainingtime == 0)
    {
        kill(getppid(),SIGUSR1);
    }
    destroyClk(false);
    
    return 0;
}
