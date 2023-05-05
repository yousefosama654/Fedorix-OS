#include "headers.h"

#include <stdio.h>
#include <stdlib.h>
#ifndef CIRCULAR_QUEUE_H_
#define CIRCULAR_QUEUE_H_
#define MAX_QUEUE_SIZE 100

// Structure to represent a priority queue element
typedef struct CircularQueueElement
{

    PCB process;
    struct CircularQueueElement *next;
} CircularQueueElement;
// Structure to represent a priority queue
typedef struct
{
    CircularQueueElement *head;
    CircularQueueElement *tail;
} CircularQueue;

// Initialize an empty queue
void q_init(CircularQueue *q)
{
    q->head = NULL;
    q->tail = NULL;
}

// Check if the queue is empty
int q_empty(CircularQueue *q)
{
    return (q->head == NULL && q->tail == NULL);
}


void display(CircularQueue * q)
{

CircularQueueElement *temp = q->head;
while(temp!=NULL){
    printf("data  %d \n", temp->process.id);
temp=temp->next;
}
}

// Check if the queue is full
// int isQueueFull(Queue* q) {
//     Node* temp = (Node*)malloc(sizeof(Node));
//     if (temp == NULL) {
//         return 1;
//     }
//     free(temp);
//     return 0;
// }

// Add an element to the queue
void q_push(CircularQueue *q, PCB data)
{
    CircularQueueElement *newNode = (CircularQueueElement *)malloc(sizeof(CircularQueueElement));

    newNode->process = data;
    newNode->next = NULL;

    if (q_empty(q)==1)
    {
        q->head = newNode;
        q->tail = newNode;
        return;
    }
    q->tail->next = newNode;
    q->tail = newNode;
}

// Remove an element from the queue
PCB q_pop(CircularQueue *q)
{
    PCB p;
    if (q_empty(q))
    {
        printf("Error: Queue is empty.\n");
        return p;
    }
    CircularQueueElement *temp = q->head;

    PCB data = temp->process;

    q->head = q->head->next;
    if (q->head == NULL)
    {
        q->tail = NULL;
    }
    free(temp);
    return data;
}


bool pop_object(CircularQueue * q,CircularQueueElement*p)
{
   if(q->head==NULL)
   
   {
     if (q->head == NULL)
    {
        q->tail = NULL;
    }
     return false;
   }
   CircularQueueElement *prev = NULL;
   CircularQueueElement *temp = q->head;
   while(temp!=NULL)
   {
     if(temp==p)
     {
        if(prev==NULL)
        
        {

             q->head=q->head->next;
        }
     
        else prev->next=temp->next;
    if (q->head == NULL)
    {
        q->tail = NULL;
    }
        return true;
     }
     else{
        prev=temp;
        temp=temp->next;
     }
   }
        if (q->head == NULL)
    {
        q->tail = NULL;
    }
    return true;
}



#endif /* PRIORITY_QUEUE_H_ */