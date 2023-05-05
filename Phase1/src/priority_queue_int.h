#include "headers.h"
#ifndef PRIORITY_QUEUE_INT_H_
#define PRIORITY_QUEUE_INT_H_
// Structure to represent a priority queue element
typedef struct PriorityQueueElementInt
{
    // int priority;
    // int data;
    int addr;
    struct PriorityQueueElementInt *next;
} PriorityQueueElementInt;
// Structure to represent a priority queue
typedef struct
{
    PriorityQueueElementInt *head;
} PriorityQueueInt;

int pq_emptyInt(PriorityQueueInt *pq)
{
    if (pq->head == NULL)
    {
        return true;
    }
    else
        return false;
}
void InitInt(PriorityQueueInt *p)
{

    p->head=NULL;
}
// Function to add an element to the priority queue
void pq_pushInt(PriorityQueueInt *pq, int data)
{
    PriorityQueueElementInt *newElement = malloc(sizeof(PriorityQueueElementInt));
    // newElement->data = priority;
    newElement->addr = data;
    newElement->next = NULL;
    if (pq->head == NULL)
    {
        pq->head = newElement;
    }
    else if (data < pq->head->addr)
    {
        newElement->next = pq->head;
        pq->head = newElement;
    }
    else
    {
        PriorityQueueElementInt *current = pq->head;
        while (current->next != NULL && current->next->addr <= data)
        {
            current = current->next;
        }
        newElement->next = current->next;
        current->next = newElement;
    }
}

// Function to remove the element with the highest priority from the priority queue
int pq_popInt(PriorityQueueInt *pq)
{
    if (pq_emptyInt(pq) == false)
    {
        int result = pq->head->addr;
        PriorityQueueElementInt *temp = pq->head;
        pq->head = pq->head->next;
        free(temp);
        return result;
    }
}
int peekInt(PriorityQueueInt *pq)
{
    if(pq_emptyInt(pq)==0)
    
    return pq->head->addr;
}

bool pop_object_int(PriorityQueueInt * q,PriorityQueueElementInt*p)
{
   if(q->head==NULL) return false;
   PriorityQueueElementInt *prev = NULL;
   PriorityQueueElementInt *temp = q->head;

   while(temp!=NULL)
   {
     if(temp==p)
     {
        if(prev==NULL)q->head=q->head->next;
        else prev->next=temp->next;

      //  free(temp);
         return true;
     }
     else{
        prev=temp;
        temp=temp->next;
     }
   }
}


#endif /* PRIORITY_QUEUE_H_ */