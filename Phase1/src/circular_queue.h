#include "headers.h"

#include <stdio.h>
#include <stdlib.h>
#ifndef CIRCULAR_QUEUE_H_
#define CIRCULAR_QUEUE_H_
#define MAX_QUEUE_SIZE 100

// Structure to represent a priority queue element
typedef struct CircularQueueElement
{
    
    processData process;
    struct CircularQueueElement *next;
} CircularQueueElement;
// Structure to represent a priority queue
typedef struct
{
    CircularQueueElement *head;
     CircularQueueElement *tail;
} CircularQueue;



// Initialize an empty queue
void q_init(CircularQueue* q) {
    q->head = NULL;
    q->tail = NULL;
}

// Check if the queue is empty
int q_empty(CircularQueue* q) {
    return (q->head == NULL && q->tail == NULL);
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
void q_push(CircularQueue* q, processData data) {
    CircularQueueElement* newNode = (CircularQueueElement*)malloc(sizeof(CircularQueueElement));
  
 
    newNode->process = data;
    newNode->next = NULL;
    
    if (q_empty(q)==1) {
         printf("\ngdg\n");
        q->head = newNode;
        q->tail = newNode;
         printf("\n %d \n",q->tail->process.id);
        return;
    }
    q->tail->next = newNode;
    q->tail = newNode;
   
    printf("\n %d \n",q->tail->process.id);
}

// Remove an element from the queue
processData q_pop(CircularQueue* q) {
    processData p;
    if (q_empty(q)) {
        printf("Error: Queue is empty.\n");
        return p;
    }
    CircularQueueElement* temp = q->head;
  
  printf("\nfghjk\n");
  printf("\n %d \n",q->head->process.id);
    processData data = temp->process;
      printf("\nfghjk\n");
    q->head = q->head->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    free(temp);
    return data;
}


#endif /* PRIORITY_QUEUE_H_ */