#include "headers.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
/*
    how it should work
    1. Start a new process. (Fork it and give it its parameters.)
    2. Switch between two processes according to the scheduling algorithm. (Stop the
    old process and save its state and start/resume another one.)
    3. Keep a process control block (PCB) for each process in the system. A PCB
    should keep track of the state of a process; running/waiting, execution time,
    remaining time, waiting time, etc.
    4. Delete the data of a process when it gets notifies that it finished. When a
    process finishes it should notify the scheduler on termination, the scheduler
    does NOT terminate the process.
    5. Report the following information
        (a) CPU utilization.
        (b) Average weighted turnaround time.
        (c) Average waiting time.
        (d) Standard deviation for average weighted turnaround time.
    6. Generate two files: (check the input/output section below)
        (a) Scheduler.log
        (b) Scheduler.perf
    */
void handler(int signum);
bool No_new_process = false;
int algo_type;
int msg_q_id;
struct Observer
{
    // the output file data loader
};
struct Entry
{
    int id;
    int pid;
    int arrivalTime;
    int lastStart;
    int remainTime;
    int runTime;
    int waitTime;
    int lastEnd;
    int priority;
    char state;
};

struct Node
{
    struct Entry entry;
    struct Node *next;
};
struct Node *head = NULL;
struct Node *rear = NULL;
struct Node *round_roubin_coming = NULL;
void push_back(struct Entry entry)
{
    struct Node *newEntry = (struct Node *)malloc(sizeof(struct Node));
    newEntry->entry = entry;
    if (head == NULL)
    {
        head = newEntry;
        rear = newEntry;
    }
    else
    {
        rear->next = newEntry;
        rear = newEntry;
    }
}
struct Node* findRunning();
struct Node* remove_running();
void insert_Queue(struct Entry entry)
{
    struct Node* newEntry = (struct Node*) malloc(sizeof(struct Node));
    newEntry->entry = entry;
    if(head == NULL)
    {
        head = newEntry;
    }
    else
    {
        struct Node* curr = head;
        struct Node* prev = NULL;
        if(algo_type == 0)
        {
            while(curr != NULL && entry.priority > curr->entry.priority)
            {
                prev = curr;
                curr = curr->next;
            }
            if(curr == head)
            {
                newEntry->next = head->next;
                head->next = newEntry;
            }
            else
            {
                prev->next = newEntry;
                newEntry->next = curr;
            }
        }
        else if(algo_type == 1)
        {
            while(curr != NULL && entry.remainTime >= curr->entry.remainTime)
            {
                prev = curr;
                curr = curr->next;
            }
            if(curr == head)
            {
                newEntry->next = head;
                head = newEntry;
            }
            else
            {
                prev->next = newEntry;
                newEntry->next = curr;
            }
        }
    }
}

void translate(char inp[])
{
    char str[strlen(inp)];
    strcpy (str, inp);
    int n = 1;//atoi(str[0]);
    for(int i = 0; i < n; i++)
    {
        //kill(getpid(),SIGSTOP);
        struct Entry entry;
        char delim[] = " ";

        // read the id
        char *ptr = strtok(str, delim);
        entry.id = atoi(ptr);
        //printf("ID: %d \n", entry.id);
        // read the arrival
        ptr = strtok(NULL, delim);
        entry.arrivalTime = atoi(ptr);
        //printf("arrival: %d \n", entry.arrivalTime);
        // read the runtime
        ptr = strtok(NULL, delim);
        entry.runTime = atoi(ptr);
        entry.remainTime = atoi(ptr);
        //printf("RunTime: %d \n", entry.runTime);
        // read the priority
        ptr = strtok(NULL, delim);
        entry.priority = atoi(ptr);
        entry.state = 'S';
        //printf("Priority: %d \n", entry.priority);
        entry.pid = -1;
        entry.waitTime = 0;
        entry.lastEnd = 0;
        if(algo_type == 2)
            push_back(entry);
        else
            insert_Queue(entry);
    }
}

void display()
{
    struct Node *temp = head;
    printf("==================START OF PCB==================\n");
    while(temp)
    {
        printf("id: %d \n",temp->entry.id);
        printf("arrivalTime: %d \n",temp->entry.arrivalTime);
        printf("pid: %d \n",temp->entry.pid);
        printf("runTime: %d \n",temp->entry.runTime);
        printf("waitTime: %d \n",temp->entry.waitTime);
        printf("remainTime: %d \n",temp->entry.remainTime);
        printf("priority: %d \n",temp->entry.priority);
        printf("state: %c \n",temp->entry.state);
        printf("====================================\n");
        temp = temp->next;
    }
    printf("==================END OF PCB==================\n");
}

FILE *out_file;
int clk;
int main(int argc, char *argv[])
{
    initClk();

    clk = getClk();
    int prevClk = 0;

    signal (SIGUSR1, handler);

    algo_type = atoi(argv[0]);
    int quantum = atoi(argv[1]);
    out_file = fopen("scheduler.log", "w"); // write only
    if (out_file == NULL) 
    {   
        printf("Error! Could not open file\n"); 
        exit(-1); // must include stdlib.h 
    }
    // write to file vs write to screen 
    fprintf(out_file, "#At time x process y state arr w total z remain y wait k\n"); // write to file
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    printf("schedular is started\n");
    key_t key_id;
    int rec_val, msgq_id;
    key_id = ftok("keyfile", 65);               //create unique key
    msgq_id = msgget(key_id, 0666 | IPC_CREAT); //create message queue and return id
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    printf("Message Queue ID = %d\n", msgq_id);

    struct msgbuff message;
    while (1)
    {   
        clk = getClk();
        rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext), 0, IPC_NOWAIT);
        if (rec_val != -1)
        {
            ////if there is no new processes
            // if (!strcmp(message.mtext, "pass"))
            // {
                
            // }
            if (!strcmp(message.mtext, "Done !"))
            {
                printf("wohooo !\n");
                No_new_process = true;
            }
            else
            {

                printf("\nMessage received: %s with m-type: %ld \n", message.mtext, message.mtype);
                translate(message.mtext);
            }
        }
        if(clk != prevClk)
        {
            printf("The  clk is %d\n",clk);
            //display();
            prevClk = clk;
            if (head == NULL && No_new_process)
                break;
            // Either HPF or SRTN
            if(algo_type != 2 && head != NULL)
            {
                // this process is first time to run
                if(head->entry.pid == -1)
                {
                    struct Node* temp = findRunning();
                    //preemption is occuring
                    if( temp != NULL)
                    {
                        kill(temp->entry.pid,SIGSTOP);
                        temp->entry.remainTime -= (clk - temp->entry.lastStart);
                        temp->entry.lastEnd = clk;
                        temp->entry.state = 'S';
                        fprintf(out_file, "At time %d process %d stoped arr %d total %d remain %d wait %d\n",clk,temp->entry.id,temp->entry.arrivalTime,temp->entry.runTime,temp->entry.remainTime,temp->entry.waitTime); // write to file
                    }
                    // // Context switching
                    // //while loop on running process
                    // // found kill(current->entry.pid,SIGSTOP)
                    // // ovserv.state stopped
                    // // current->entry.r = r-(clk - lst)
                    int pid = fork();
                    if(pid == 0)
                    {
                        printf(" I am going to fork....");
                        //head->entry.waitTime= (clk - head->entry.arrivalTime) - (head->entry.runTime-head->entry.remainTime);
                        //execl("./process.out", head->entry.remainTime);//, head->entry.waitTime);
                        char s_remain_time[10];
                        sprintf(s_remain_time, "%d", head->entry.remainTime);
                        execl("./process.out", s_remain_time);
                        printf("error is %d\n", errno);
                        printf("error occurred couldn't execute process.out");
                        exit(-1);
                    }
                    // parent (scheduler) is modifying the pcb
                    else
                    {
                        head->entry.pid = pid;
                        head->entry.state = 'R';
                        head->entry.lastStart = clk;
                        head->entry.waitTime = clk - head->entry.arrivalTime;
                        fprintf(out_file, "At time %d process %d started arr %d total %d remain %d wait %d\n",clk,head->entry.id,head->entry.arrivalTime,head->entry.runTime,head->entry.remainTime,head->entry.waitTime); // write to file
                        // Saving the status for the system preparing to outfile
                    }
                }
                // Preempted before and Now its time to continue
                else
                {
                    // //SIG CONT & lst = clk & save data
                    struct Node* temp = findRunning();
                    // Preemption will occur between the temp(the running before) and head (that will run)
                    if(head->entry.state != 'R')
                    {
                        if(temp != NULL)
                        {
                            kill(temp->entry.pid,SIGSTOP);
                            temp->entry.state = 'S';
                            temp->entry.lastEnd = clk;
                            temp->entry.remainTime -= (clk - temp->entry.lastStart);
                            fprintf(out_file, "At time %d process %d stopped arr %d total %d remain %d wait %d\n",clk,temp->entry.id,temp->entry.arrivalTime,temp->entry.runTime,temp->entry.remainTime,temp->entry.waitTime); // write to file
                        }
                        head->entry.state = 'R';
                        head->entry.lastStart = clk;
                        head->entry.waitTime += clk - head->entry.lastEnd;
                        kill(head->entry.pid,SIGCONT);
                        fprintf(out_file, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",clk,head->entry.id,head->entry.arrivalTime,head->entry.runTime,head->entry.remainTime,head->entry.waitTime); // write to file

                    }
                    // No Preemption will occur as Head still Qualified
                    else
                    {
                        head->entry.remainTime -= (clk - head->entry.lastStart);
                        head->entry.lastEnd = clk;
                        head->entry.lastStart = clk;
                        //head->entry.remainTime -= 1;// = head->entry.runTime - (clk - head->entry.lastStart);
                    }
                }
            }
            // Round Roubin
            else if (head != NULL)
            {
                struct Node* temp = findRunning();
                // there is a finished process or this is the first step in the system
                if(temp == NULL)
                {
                    // there is a finished process *NOT* at the end of the pcb
                    if(round_roubin_coming != NULL)
                    {
                        // it is first time to run
                        if(round_roubin_coming->entry.pid == -1)
                        {
                            int pid = fork();
                            if(pid == 0)
                            {
                                printf(" I am going to fork....");
                                char s_remain_time[10];
                                sprintf(s_remain_time, "%d", round_roubin_coming->entry.remainTime);
                                execl("./process.out", s_remain_time);
                                printf("error is %d\n", errno);
                                printf("error occurred couldn't execute process.out");
                                exit(-1);
                            }
                            else
                            {
                                round_roubin_coming->entry.waitTime = clk - round_roubin_coming->entry.arrivalTime;
                                round_roubin_coming->entry.pid = pid;
                                round_roubin_coming->entry.lastStart = clk;
                                round_roubin_coming->entry.state = 'R';
                                fprintf(out_file, "At time %d process %d started arr %d total %d remain %d wait %d\n",clk,round_roubin_coming->entry.id,round_roubin_coming->entry.arrivalTime,round_roubin_coming->entry.runTime,round_roubin_coming->entry.remainTime,round_roubin_coming->entry.waitTime); // write to file
                                round_roubin_coming = round_roubin_coming->next;
                            }
                        }
                        // its run before
                        else
                        {
                            round_roubin_coming->entry.waitTime += clk - round_roubin_coming->entry.lastEnd;
                            round_roubin_coming->entry.lastStart = clk;
                            round_roubin_coming->entry.state = 'R';
                            kill(round_roubin_coming->entry.pid,SIGCONT);
                            fprintf(out_file, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",clk,round_roubin_coming->entry.id,round_roubin_coming->entry.arrivalTime,round_roubin_coming->entry.runTime,round_roubin_coming->entry.remainTime,round_roubin_coming->entry.waitTime); // write to file
                            round_roubin_coming = round_roubin_coming->next;
                        }
                    }
                    else
                    {
                        // it is first time to run
                        if(head->entry.pid == -1)
                        {
                            int pid = fork();
                            if(pid == 0)
                            {
                                printf(" I am going to fork....");
                                char s_remain_time[10];
                                sprintf(s_remain_time, "%d", head->entry.remainTime);
                                execl("./process.out", s_remain_time);
                                printf("error is %d\n", errno);
                                printf("error occurred couldn't execute process.out");
                                exit(-1);
                            }
                            else
                            {
                                head->entry.waitTime = clk - head->entry.arrivalTime;
                                head->entry.pid = pid;
                                head->entry.lastStart = clk;
                                head->entry.state = 'R';
                                fprintf(out_file, "At time %d process %d started arr %d total %d remain %d wait %d\n",clk,round_roubin_coming->entry.id,round_roubin_coming->entry.arrivalTime,round_roubin_coming->entry.runTime,round_roubin_coming->entry.remainTime,round_roubin_coming->entry.waitTime); // write to file
                                round_roubin_coming = head->next;
                            }
                        }
                        // its run before
                        else
                        {
                            head->entry.waitTime += clk - head->entry.lastEnd;
                            head->entry.lastStart = clk;
                            head->entry.state = 'R';
                            kill(round_roubin_coming->entry.pid,SIGCONT);
                            fprintf(out_file, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",clk,round_roubin_coming->entry.id,round_roubin_coming->entry.arrivalTime,round_roubin_coming->entry.runTime,round_roubin_coming->entry.remainTime,round_roubin_coming->entry.waitTime); // write to file
                            round_roubin_coming = head->next;
                        }
                    }
                }
                else
                {
                // there is running process found: 1-Quantum hasnot finished 2-Quantum finished and no other process 3-Quantum finished and preemption will occur
                    // took a complete quantum
                    if(clk - temp->entry.lastStart == quantum)
                    {
                        // there is only one process in the system
                        if(temp->entry.pid == head->entry.pid && round_roubin_coming == NULL)
                        {
                            temp->entry.remainTime -= quantum;
                            temp->entry.lastStart = clk;
                            round_roubin_coming = temp->next;
                        }
                        else
                        {
                            kill(temp->entry.pid,SIGSTOP);
                            temp->entry.lastEnd = clk;
                            temp->entry.remainTime -= quantum;
                            temp->entry.state = 'S';
                            fprintf(out_file, "At time %d process %d stopped arr %d total %d remain %d wait %d\n",clk,round_roubin_coming->entry.id,round_roubin_coming->entry.arrivalTime,round_roubin_coming->entry.runTime,round_roubin_coming->entry.remainTime,round_roubin_coming->entry.waitTime); // write to file
                            if(round_roubin_coming != NULL)
                            {
                                //first time to be runned
                                if(round_roubin_coming->entry.pid == -1)
                                {
                                    int pid = fork();
                                    if(pid == 0)
                                    {
                                        printf(" I am going to fork....");
                                        char s_remain_time[10];
                                        sprintf(s_remain_time, "%d", round_roubin_coming->entry.remainTime);
                                        execl("./process.out", s_remain_time);
                                        printf("error is %d\n", errno);
                                        printf("error occurred couldn't execute process.out");
                                        exit(-1);
                                    }
                                    else
                                    {
                                        round_roubin_coming->entry.pid = pid;
                                        round_roubin_coming->entry.state = 'R';
                                        round_roubin_coming->entry.lastStart = clk;
                                        round_roubin_coming->entry.waitTime = clk - round_roubin_coming->entry.arrivalTime;
                                        fprintf(out_file, "At time %d process %d started arr %d total %d remain %d wait %d\n",clk,round_roubin_coming->entry.id,round_roubin_coming->entry.arrivalTime,round_roubin_coming->entry.runTime,round_roubin_coming->entry.remainTime,round_roubin_coming->entry.waitTime); // write to file
                                        round_roubin_coming = round_roubin_coming->next;
                                        // Saving the status for the system preparing to outfile
                                    }
                                }
                                else
                                {
                                    round_roubin_coming->entry.state = 'R';
                                    round_roubin_coming->entry.lastStart = clk;
                                    round_roubin_coming->entry.waitTime += (clk - round_roubin_coming->entry.lastEnd);
                                    kill(round_roubin_coming->entry.pid,SIGCONT);
                                    fprintf(out_file, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",clk,round_roubin_coming->entry.id,round_roubin_coming->entry.arrivalTime,round_roubin_coming->entry.runTime,round_roubin_coming->entry.remainTime,round_roubin_coming->entry.waitTime); // write to file
                                    round_roubin_coming = round_roubin_coming->next;
                                } 
                            }
                            else
                            {
                                head->entry.state = 'R';
                                head->entry.lastStart = clk;
                                round_roubin_coming = head->next;
                                head->entry.waitTime += clk - head->entry.lastEnd;
                                kill(head->entry.pid,SIGCONT);
                                fprintf(out_file, "At time %d process %d started arr %d total %d remain %d wait %d\n",clk,head->entry.id,head->entry.arrivalTime,head->entry.runTime,head->entry.remainTime,head->entry.waitTime); // write to file
                            }
                        }
                    }
                }
            }
            display();
        }
    }
    fclose(out_file);
    destroyClk(true);
}
struct Node* findRunning()
{
   //start from the first link
   struct Node* current = head;

   //if list is empty
   if(head == NULL)
   {
      return NULL;
   }

   //navigate through list
   while(current->entry.state != 'R')
   {
      //if it is last node
      if(current->next == NULL)
      {
            return NULL;
      } 
      else
      {
            //go to next link
            current = current->next;
      }
   }      
	
   return current;
}

struct Node* remove_running()
{
   struct Node* current = head;
   struct Node* previous = NULL;
   if(head == NULL)
   {
      return NULL;
   }

   while(current->entry.state != 'R')
   {
        if(current->next == NULL)
        {
            return NULL;
        } 
        else 
        {
            previous = current;
            current = current->next;
        }
   }
   if(current == head)
   {
      head = head->next;
   } 
   else
   {
      previous->next = current->next;
   }    
   return current;
}
// void delete_first()
// {
//     struct Node* temp = head;
//     head = head->next;
// }
// void process_finished()
// {
//     if(algo_type == 2)
//         remove_running();
//     else
//         delete_first();
// }
void handler(int signum)
{
    printf("Schedular have got signal #%d from a finished process\n", signum);
    //process_finished();
    struct Node* temp = remove_running();
    fprintf(out_file, "At time %d process %d finished arr %d total %d remain %d wait %d\n",clk,temp->entry.id,temp->entry.arrivalTime,temp->entry.runTime,temp->entry.remainTime,temp->entry.waitTime); // write to file
    
    signal(SIGUSR1, handler );
}