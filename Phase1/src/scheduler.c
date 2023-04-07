#include "headers.h"
#include "priority_queue.h"
PriorityQueue *PriorityQueueHPF = NULL;
void SignalINThandler(int signnum);
PriorityQueue *getPrioityQueueHPF()
{
    // Create shared memory for PriorityQueueHPF
    int pqid = shmget(PQKEY, sizeof(PriorityQueue), IPC_CREAT | 0644);
    if ((long)pqid == -1)
    {
        perror("Error in creating priority queue!");
        exit(-1);
    }
    PriorityQueue *pqaddr = (PriorityQueue *)shmat(pqid, (void *)0, 0);
    if ((long)pqaddr == -1)
    {
        perror("Error in attaching the priority queue!");
        exit(-1);
    }
    return pqaddr;
}

void HPF()
{
    while (pq_empty(PriorityQueueHPF) != false)
    {
        PCB process = pq_pop(PriorityQueueHPF);
        printf("pid id %d has arrived safely\n", process.id);
        // int pid = fork();
        // if (pid == 0)
        // {
        //     execl("./bin/process.out", "process.out", NULL);
        // }
        // else
        // {

        // }
    }
}
void SRTN()
{
}
void RR()
{
}
int main(int argc, char *argv[])
{
    signal(SIGINT, SignalINThandler);
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
        printf("Highest Prioity First Next Algorithm starting...\n");
        PriorityQueueHPF = getPrioityQueueHPF();
        while (1)
        {
            HPF();
        }
    }

    // TODO implement the scheduler :)
    // upon termination release the clock resources.
    // destroyClk(true);
}
void SignalINThandler(int signnum)
{
    free(PriorityQueueHPF);
    exit(0);
}