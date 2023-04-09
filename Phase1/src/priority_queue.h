#include "headers.h"
#ifndef PRIORITY_QUEUE_H_
#define PRIORITY_QUEUE_H_
// Structure to represent a priority queue element
typedef struct PriorityQueueElement
{
    PCB process;
    struct PriorityQueueElement *next;
} PriorityQueueElement;
// Structure to represent a priority queue
typedef struct
{
    PriorityQueueElement *head;
} PriorityQueue;
void pq_init(PriorityQueue *pq)
{
    pq->head = NULL;
}
// Function to add an element to the priority queue
void pq_push(PriorityQueue *pq, PCB data)
{
    // printf("start the push function\n");
    PriorityQueueElement *newElement = malloc(sizeof(PriorityQueueElement));
    // printf("malloced sucess\n");
    newElement->process = data;
    // printf("made the process suc\n");
    newElement->next = NULL;
    // printf("set the null ptr\n");
    // printf("the pid is = %d with arrival time = %d\n", newElement->process.id, newElement->process.arrivaltime);
    if (pq->head == NULL)
    {
        // printf(" iam in the if cond\n");
        pq->head = newElement;
    }
    else if (data.priority < pq->head->process.priority)
    {
        // printf("i am in the special cond\n");
        newElement->next = pq->head;
        pq->head = newElement;
    }
    else
    {
        // printf("i am in the general cond\n");
        PriorityQueueElement *current = pq->head;
        while (current->next != NULL && current->next->process.priority <= data.priority)
        {
            current = current->next;
        }
        newElement->next = current->next;
        current->next = newElement;
    }
}
int pq_empty(PriorityQueue *pq)
{
    if (pq->head == NULL)
    {
        return true;
    }
    else
        return false;
}
// Function to remove the element with the highest priority from the priority queue
PCB pq_pop(PriorityQueue *pq)
{
    if (pq_empty(pq) == false)
    {
        PCB result = pq->head->process;
        PriorityQueueElement *temp = pq->head;
        pq->head = pq->head->next;
        free(temp);
        return result;
    }
}
void pq_display(PriorityQueue *pq)
{
    PriorityQueueElement *temp = pq->head;
    while (temp != NULL)
    {
        PCB foo = temp->process;
        printf("pid %d with priority %d\n", foo.id, foo.priority);
        temp = temp->next;
    }
}
#endif /* PRIORITY_QUEUE_H_ */