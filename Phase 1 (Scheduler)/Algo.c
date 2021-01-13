#include "headers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
//#include <errno.h>
struct Process
{
    int id;
    int arrival_time;
    bool arrived;
    int runtime;
    int remaintime;
    int priority; // this is process priority ranges from 0 to 10 where 0 is the heighest priority
};
void RR(struct Process arr[], int quantum)
{
    int clk = 0;
    int size = sizeof(arr);
    //queue<struct Process> q;
    queue<struct Process> q;
    while (size > 0)
    {
        q
    }
}

void HPF(struct Process arr[] )
{
    
}

void SRTN(struct Process arr[] )
{
    
}
void main()
{
    int clk = 0;

}
/*#At time x p r o c e s s y s t a t e a r r w t o t a l z remain y w a i t k
   At time 1 p r o c e s s 1 s t a r t e d a r r 1 t o t a l 6 remain 6 w a i t 0
   At time 3 p r o c e s s 1 s t o p p e d a r r 1 t o t a l 6 remain 4 w a i t 0
   At time 3 p r o c e s s 2 s t a r t e d a r r 3 t o t a l 3 remain 3 w a i t 0
   At time 6 p r o c e s s 2 f i n i s h e d a r r 3 t o t a l 3 remain 0 w a i t 0 TA 3 WTA 1
   At time 6 p r o c e s s 1 resumed a r r 1 t o t a l 6 remain 4 w a i t 3
   At time 10 p r o c e s s 1 f i n i s h e d a r r 1 t o t a l 6 remain 0 w a i t 3 TA 10 WTA 1 . 6 7*/