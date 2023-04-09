#include "headers.h"
#include "priority_queue.h"
void SignalINThandler(int signnum);
PriorityQueue *PriorityQueueHPF = NULL;
void SetPrioityQueueHPF()
{
    // Create shared memory for one PriorityQueue
    int pqid = shmget(PQKEY, sizeof(PriorityQueue), IPC_CREAT | 0644);
    if ((long)pqid == -1)
    {
        perror("Error in creating priority queue!");
        exit(-1);
    }
    PriorityQueueHPF = (PriorityQueue *)shmat(pqid, (void *)0, 0);
    PriorityQueueHPF = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    if ((long)PriorityQueueHPF == -1)
    {
        perror("Error in attaching the priority queue!");
        exit(-1);
    }
}
// PCB *currentProcessHPF = NULL;
// void getProcessHPF()
// {
//     // Create shared memory for one PriorityQueue
//     int pqid = shmget(12, sizeof(PCB), 0644);
//     if ((long)pqid == -1)
//     {
//         perror("Error in creating process HPF!");
//         exit(-1);
//     }
//     PCB *pqaddr = (PCB *)shmat(pqid, (void *)0, 0);
//     if ((long)pqaddr == -1)
//     {
//         perror("Error in attaching process HPF!");
//         exit(-1);
//     }
//     currentProcessHPF = pqaddr;
// }
// int previd = -1;
void HPF()
{
    PCB p;
    p.id = -1;
    pq_push(PriorityQueueHPF, p);
    printf("printing from scudler\n");
    pq_display(PriorityQueueHPF);
    sleep(3);
}
void SRTN()
{
}
void RR()
{
}
int main(int argc, char *argv[])
{

    // signal(SIGINT, SignalINThandler);
    initClk();
    char *Scheduling_Algo = argv[1];
    if (strcmp(Scheduling_Algo, "RR") == 0)
    {
        printf("Round Robin Algorithm starting...\n");
        while (1)
        {
            RR();
        }
    }
    else if (strcmp(Scheduling_Algo, "SRTN") == 0)
    {
        printf("Shortest Remaining Time Next Algorithm starting...\n");
        while (1)
        {
            SRTN();
        }
    }
    else
    {
        printf("Highest Prioity First Algorithm starting...\n");
        SetPrioityQueueHPF();
        while (1)
        {
            HPF();
        }
    }
}
void SignalINThandler(int signnum)
{
    free(PriorityQueueHPF);
    killpg(getpgrp(), SIGINT);
    exit(0);
}