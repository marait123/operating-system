#include "headers.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>

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
    struct MemNode *memory;
};

struct Node
{
    struct Entry entry;
    struct Node *next;
};
// this is for waiting queue

struct Node *waiting_head = NULL;
struct Node *waiting_rear = NULL;

// push in the place where every one behind it
// is bigger
void push_in_waiting(struct Entry entry)
{
    struct Node *newEntry = (struct Node *)malloc(sizeof(struct Node));
    newEntry->entry = entry;

    if (waiting_head == NULL)
    {
        waiting_head = newEntry;
        waiting_rear = newEntry;
    }
    else
    {
        struct Node *scan = waiting_head;
        while (scan->next != NULL)
        {
            if (scan->next->entry.memory->length >= entry.memory->length)
            {
                scan->next = newEntry;
                break;
            }
            scan = scan->next;
        }
        if (scan == waiting_head)
        {
        }
    }
}

struct MemNode
{
    int begin;
    int length;
    char state; // H or P
    struct MemNode *next;
};

struct MemNode *Mem_Head = NULL;
/*
return the MemNode with the least suitable length 
or returns NULL if not found
*/
struct MemNode *suitable_memory(int length)
{
    length = next_pow_2(length);

    struct MemNode *scan = Mem_Head;
    struct MemNode *Min_Hole = NULL;
    while (scan)
    {
        if (scan->state == 'H')
        {
            if (scan->length >= length)
            {
                // found the suitalbe hole
                Min_Hole = scan;
                scan = scan->next;
                while (scan)
                {
                    if (scan->state == 'H')
                    {
                        if (scan->length < Min_Hole->length && scan->length >= length)
                        {
                            Min_Hole = scan;
                        }
                    }
                    scan = scan->next;
                }

                break;
            }
        }
        scan = scan->next;
    }
    return Min_Hole;
}

struct MemNode *insert_memory(struct MemNode *start, int length)
{
    int block_size = next_pow_2(length);
    // if (block)
    // start is divided into 2 parts
    // the first part is the process
    // the second part is the hole if there still a size
    struct MemNode *process_place = NULL;

    if (block_size == start->length)
    {
        start->state = 'P';
        process_place = start;
    }
    else if (start->length > block_size)
    {

        while (block_size != start->length)
        {
            struct MemNode *temp = NULL;
            temp = (struct MemNode *)malloc(sizeof(struct MemNode));
            temp->begin = start->begin + (start->length) / 2;
            temp->state = 'H';
            temp->length = (start->length) / 2;
            temp->next = start->next;
            // change start
            start->next = temp;
            start->length = (start->length) / 2;
        }

        process_place = start;
        process_place->state = 'P';
    }

    else
    {
        perror("an error occured in memroy insert");
        exit(-1);
    }
    if (process_place == NULL)
    {
        perror("an error occured process_place in memroy insert");
        exit(-1);
    }

    return process_place;
}

unsigned next_pow_2(unsigned size)
{
    size -= 1;
    size |= (size >> 1);
    size |= (size >> 2);
    size |= (size >> 4);
    size |= (size >> 8);
    size |= (size >> 16);
    return size + 1;
}

int main()
{

    Mem_Head = (struct MemNode *)malloc(sizeof(struct MemNode));
    Mem_Head->begin = 0;
    Mem_Head->state = 'H';
    Mem_Head->length = 1024;
    Mem_Head->next = NULL;

    return 0;
}
