#include "headers.h"
#include <errno.h>
#include <string.h>
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
struct msgbuff
{
    long mtype;
    char mtext[256];
};
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
    int waitTime;
    int runTime;
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
void push_back(struct Entry entry)
{
    struct Node *newEntry = (struct Node *)malloc(sizeof(struct Node));
    newEntry->entry = entry;
    if (head == NULL)
    {
        head = newEntry;
    }
    else
    {
        struct Node *temp = rear;
        temp->next = newEntry;
        rear = newEntry;
    }
}
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
            while(entry.priority > curr->entry.priority)
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
            while(entry.remainTime > curr->entry.remainTime)
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

void translate(char str[])
{
    int n = 1;//atoi(str[0]);
    for(int i = 0; i < n; i++)
    {
        //kill(getpid(),SIGSTOP);
        struct Entry entry;
        char delim = " ";

        // read the id
        char *ptr = strtok(str, delim);
        entry.id = atoi(ptr);

        // read the arrival
        ptr = strtok(NULL, delim);
        entry.arrivalTime = atoi(ptr);

        // read the runtime
        ptr = strtok(NULL, delim);
        entry.runTime = atoi(ptr);
        entry.remainTime = atoi(ptr);

        // read the priority
        ptr = strtok(NULL, delim);
        entry.priority = atoi(ptr);
        entry.pid = -1;
        if(algo_type == 2)
            push_back(entry);
        else
            insert_Queue(entry);
    }
}
void display()
{
    struct Node *temp = head;
    while(temp)
    {
        printf("id: ",temp->entry.id);
        printf("arrivalTime: ",temp->entry.arrivalTime);
        printf("pid: ",temp->entry.pid);
        printf("runTime: ",temp->entry.runTime);
        printf("waitTime: ",temp->entry.waitTime);
        printf("remainTime: ",temp->entry.remainTime);
        printf("priority: ",temp->entry.priority);
        printf("state: ",temp->entry.state);
        printf("====================================");
        temp = temp->next;
    }
}
int main(int argc, char *argv[])
{
    initClk();

    int clk = getClk();
    int prevClk = 0;

    signal (SIGUSR1, handler);

    algo_type = atoi(argv[0]);
    int quantum = atoi(argv[1]);
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

        if(clk != prevClk)
        {
            prevClk = clk;
            if (head == NULL && No_new_process)
                break;
            rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext), 0, IPC_NOWAIT);
            if (rec_val != -1)
            {
                ////if there is no new processes
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
            // Either HPF or SRTN
            if(algo_type != 2 && head != NULL)
            {
                if(head->entry.pid == -1)
                {   
                    struct Node* temp = findRunning();
                    if( temp != NULL)
                    {
                        kill(temp->entry.pid,SIGSTOP);
                        temp->entry.remainTime -= (clk - temp->entry.lastStart);
                        temp->entry.state = 'S';
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
                        execl ("./process.out", head->entry.remainTime);
                        printf("error is %d\n", errno);
                        printf("error occurred couldn't execute process.out");
                        exit(-1);
                    }
                    else
                    {
                        head->entry.pid = pid;
                        head->entry.state = 'R';
                        head->entry.lastStart = clk;
                        // Saving the status for the system preparing to outfile
                    }
                }
                else
                {
                    // //SIG CONT & lst = clk & save data
                    struct Node* temp = findRunning();
                    if(head->entry.state != 'R')
                    {
                        kill(temp->entry.pid,SIGSTOP);
                        temp->entry.remainTime -= (clk - temp->entry.lastStart);
                        temp->entry.state = 'S';
                    }
                    head->entry.state = 'R';
                    head->entry.lastStart = clk;
                    kill(head->entry.pid,SIGCONT);
                }
            }
            // Round Roubin
            else if (head != NULL)
            {
                struct Node* temp = findRunning();
                if(temp == NULL)
                {
                    int pid = fork();
                    if(pid == 0)
                    {
                        printf(" I am going to fork....");
                        execl ("./process.out", head->entry.remainTime);
                        printf("error is %d\n", errno);
                        printf("error occurred couldn't execute process.out");
                        exit(-1);
                    }
                    else
                    {
                        head->entry.pid = pid;
                        head->entry.state = 'R';
                        head->entry.lastStart = clk;
                        // Saving the status for the system preparing to outfile
                    }
                }
                else
                { 
                    if(clk - temp->entry.lastStart == quantum)
                    {
                        kill(temp->entry.pid,SIGSTOP);
                        temp->entry.remainTime -= quantum;
                        temp->entry.state = 'S';
                        if(temp->next != NULL)
                        {
                            if(temp->next->entry.pid == -1)
                            {
                                int pid = fork();
                                if(pid == 0)
                                {
                                    printf(" I am going to fork....");
                                    execl ("./process.out", temp->next->entry.remainTime);
                                    printf("error is %d\n", errno);
                                    printf("error occurred couldn't execute process.out");
                                    exit(-1);
                                }
                                else
                                {
                                    temp->next->entry.pid = pid;
                                    temp->next->entry.state = 'R';
                                    temp->next->entry.lastStart = clk;
                                    // Saving the status for the system preparing to outfile
                                }
                            }
                            else
                            {
                                temp->next->entry.state = 'R';
                                temp->next->entry.lastStart = clk;
                                kill(temp->next->entry.pid,SIGCONT);
                            }
                            
                        }
                        else if (head != temp)
                        {
                            head->entry.state = 'R';
                            head->entry.lastStart = clk;
                            kill(head->entry.pid,SIGCONT);
                        }
                    }
                }
            }
        }
    }

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
// struct Node* remove_entry(int index)
// {
//    struct Node* current = head;
//    struct Node* previous = NULL;
//    if(head == NULL)
//    {
//       return NULL;
//    }

//    while(current->entry.id != index)
//    {
//       if(current->next == NULL)
//       {
//          return NULL;
//       } 
//       else 
//       {
//          previous = current;
//          current = current->next;
//       }
//    }
//    if(current == head)
//    {
//       head = head->next;
//    } 
//    else
//    {
//       previous->next = current->next;
//    }    
//    return current;
// }
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
    remove_running();
    signal(SIGUSR1, handler );
}