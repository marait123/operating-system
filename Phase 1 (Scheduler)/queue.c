#include <stdio.h>
typedef short bool;
#define true 1
#define false 0
struct Process
{
    int id;
    int arrival_time;
    bool arrived;
    int runtime;
    int remaintime;
    int priority; // this is process priority ranges from 0 to 10 where 0 is the heighest priority
};
int turnarroundtime(int processes[], int n, int bt[], int wt[], int tat[])
{
   for (int i = 0; i < n ; i++)
   tat[i] = bt[i] + wt[i];
   return 1;
}

int waitingtime(int processes[], int n, int bt[], int wt[], int quantum)
{
   int rem_bt[n];
   for (int i = 0 ; i < n ; i++)
   rem_bt[i] = bt[i];
   int t = 0; 
   while (1) {
      bool done = true;
      for (int i = 0 ; i < n; i++) 
      {
            if (rem_bt[i] > 0) 
            {
                  done = false; // There is a pending process
                  if (rem_bt[i] > quantum) {
                  t += quantum;
                  rem_bt[i] -= quantum;
            }
            else 
            {
               t = t + rem_bt[i];
               wt[i] = t - bt[i];
               rem_bt[i] = 0;
            }
         }
      }
      if (done == true)
         break;
   }
   return 1;
}
int findavgTime(int processes[], int n, int bt[], int quantum)
{
   int wt[n], tat[n], total_wt = 0, total_tat = 0;
   waitingtime(processes, n, bt, wt, quantum);
   turnarroundtime(processes, n, bt, wt, tat);
   printf("Processes Burst Time Waiting Time turnaround time\n");
   for (int i=0; i<n; i++) {
      total_wt = total_wt + wt[i];
      total_tat = total_tat + tat[i];
      printf("\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",i+1, bt[i], wt[i], tat[i]);
   }
   printf("Average waiting time = %f", (float)total_wt / (float)n);
   printf("\nAverage turnaround time = %f\n", (float)total_tat / (float)n);
   return 1;
}

int main()
{
   int processes[] = { 1, 2, 3};
   int n = sizeof processes;
   int burst_time[] = {8, 6, 12};
   int quantum = 2;
   findavgTime(processes, n, burst_time, quantum);
   return 0;
}