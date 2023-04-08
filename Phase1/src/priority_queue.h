#include "headers.h"
#ifndef PRIORITY_QUEUE_H_
#define PRIORITY_QUEUE_H_
// Structure to represent a priority queue element
typedef struct PriorityQueueElement
{
    // int priority;
    // int data;
    processData process;
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
void pq_push(PriorityQueue *pq, processData data)
{
    PriorityQueueElement *newElement = malloc(sizeof(PriorityQueueElement));
    // newElement->data = priority;
    newElement->process = data;
    newElement->next = NULL;
    if (pq->head == NULL)
    {
        pq->head = newElement;
    }
    else if (data.priority < pq->head->process.priority)
    {
        newElement->next = pq->head;
        pq->head = newElement;
    }
    else
    {
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
processData pq_pop(PriorityQueue *pq)
{
    if (pq_empty(pq) == false)
    {
        processData result = pq->head->process;
        PriorityQueueElement *temp = pq->head;
        pq->head = pq->head->next;
        free(temp);
        return result;
    }
}

#endif /* PRIORITY_QUEUE_H_ */