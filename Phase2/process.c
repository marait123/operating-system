#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    // clk = 1 , remain = 5, start = 1 , last = 1
    // clk = 3 , remain = 3, start = 1 , last = 1
    // clk = 5 , remain = 3, start = 1 , last = 5
    // clk = 7 , remain = 1, start = 1 , last = 5

    // Remaining time = Runtime - ( getclk()- arrival time - waiting time )
    //Arrival time = 1, firstStart = 3, lastStart = 7
    //1->3 , 8->10, 12->14  ======     Runtime = 6, Arrival = 1 
    //waiting = firstStart - arrival + (lastStart - fisrtStart) - (runtime - remainingTime)
    // waiting = 7 - 2 = 5
    // remaining_time = getClk() - node->entry.lastStart
    //TODO it needs to get the remaining time from somewhere
    printf("Welcome in Process File\n");
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
    // finished
    kill(getppid(),SIGUSR1);
    destroyClk(false);
    
    return 0;
}
