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
    int original_size;
    struct MemNode *memory;
};

struct Node
{
    struct Entry entry;
    struct Node *next;
};
// this is for waiting queue

struct Node *waiting_head = NULL;

// push in the place where every one behind it
// is bigger
// entry must contain memory element with the length set
struct Node *pop_from_waiting()
{
    if (waiting_head == NULL)
    {
        return NULL;
    }
    struct Node *temp = waiting_head;
    waiting_head = waiting_head->next;
    return temp;
}
void push_in_waiting(struct Entry entry)
{
    struct Node *newEntry = (struct Node *)malloc(sizeof(struct Node));
    newEntry->entry = entry;

    if (waiting_head == NULL)
    {
        waiting_head = newEntry;
    }
    else
    {
        struct Node *scan = waiting_head;
        struct Node *prev = NULL;

        while (scan != NULL && scan->entry.memory->length <= entry.memory->length)
        {
            prev = scan;
            scan = scan->next;
        }
        if (prev == NULL)
        {
            newEntry->next = waiting_head;
            waiting_head = newEntry;
        }
        else
        {
            newEntry->next = scan;
            prev->next = newEntry;
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
// after releasing memory check if the length
// of the returned MemNode is suitable for the first
// Node in the waiting_list
struct MemNode *release_memory(struct MemNode *mem)
{
    struct MemNode *scan = Mem_Head;
    struct MemNode *prev = NULL;
    int next_pow = next_pow_2(mem->length + 1);
    if (mem->length % next_pow == 0)
    {
        ///merging with right
        if (mem->next->state == 'H' &&
            mem->next->length == mem->length)
        {
            // merge
            // TODO free the memeory released
            mem->length = mem->length * 2;
            mem->next = mem->next->next;
            mem->state = 'H';
        }
    }
    else
    {
        // merge with left
        while (scan != mem)
        {
            prev = scan;
            scan = scan->next;
        }
        // scan points to mem and prev points to
        // the node before scan
        if (prev == NULL)
        {

            Mem_Head = Mem_Head->next;
        }
        else
        {
        }
    }
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
