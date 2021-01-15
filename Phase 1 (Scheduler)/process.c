#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    
    //TODO it needs to get the remaining time from somewhere
    remainingtime = atoi(argv[0]);
    int clk;
    int prevClk = 0;
    while (remainingtime > 0)
    {
        clk = getClk();
        if(clk != prevClk)
        {
            remainingtime--;
            prevClk = clk;
        }
    }
    kill(getppid(),SIGUSR1);
    destroyClk(true);
    
    return 0;
}
