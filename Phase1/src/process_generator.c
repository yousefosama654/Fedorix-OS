#include "headers.h"
#include "priority_queue.h"
void clearResources(int);
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
    pq_init(PriorityQueueHPF);
}
// PCB *currentProcessHPF = NULL;
// void getProcessHPF()
// {
//     // Create shared memory for one PriorityQueue
//     int pqid = shmget(12, sizeof(PCB), IPC_CREAT | 0644);
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
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    char *Scheduling_Algo;
    FILE *pFile;
    int ProcessesNum;
    // 1. Read the input files.
    pFile = fopen("./Log/processes.txt", "r");
    if (pFile == NULL)
    {
        printf("No such file processes.txt in Log Directory\n");
        return 0;
    }
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    printf("Please choose the appropraite scheduling algorithm\n");
    fscanf(pFile, "%d", &ProcessesNum);
    PCB *Processes = (PCB *)malloc(ProcessesNum * sizeof(PCB));
    for (int i = 0; i < ProcessesNum; i++)
    {
        fscanf(pFile, "%d", &Processes[i].id);
        fscanf(pFile, "%d", &Processes[i].arrivaltime);
        fscanf(pFile, "%d", &Processes[i].runningtime);
        fscanf(pFile, "%d", &Processes[i].priority);
        Processes[i].waitingtime = 0;
    }
    scanf("%ms", &Scheduling_Algo);
    if (strcmp(Scheduling_Algo, "HPF") == 0)
    {
        SetPrioityQueueHPF();
        // printf(" i am before tha shared memory\n");
        // PriorityQueueHPF = getPrioityQueueHPF();
        // getProcessHPF();
    }
    if (strcmp(Scheduling_Algo, "SRTN") != 0 && strcmp(Scheduling_Algo, "HPF") != 0 && strcmp(Scheduling_Algo, "RR") != 0)
    {
        printf("Fedorix OS Schdeuler does not supprt this Algorithm.\n");
        return 0;
    }
    // 3. Initiate and create the scheduler and clock processes.
    int pid = fork();
    if (pid == 0)
    {
        execl("./bin/scheduler.out", "scheduler.out", Scheduling_Algo, NULL);
    }
    else
    {
        pid = fork();
        if (pid == 0)
        {
            execl("./bin/clk.out", "clk.out", NULL);
        }
        else
        {
            int i = 0;
            initClk();
            while (i < ProcessesNum)
            {
                // 4. Use this function after creating the clock process to initialize clock
                // To get time use this function
                int x = getClk();
                // printf("current time is %d\n", x);
                if (Processes[i].arrivaltime == x)
                {
                    // 6. Send the information to the scheduler at the appropriate time.
                    if (strcmp(Scheduling_Algo, "HPF") == 0)
                    {
                        // printf("process at %d\n", x);
                        // printf("found pid %d with arrival time %d at time %d\n", Processes[i].id, Processes[i].arrivaltime, x);
                        pq_push(PriorityQueueHPF, Processes[i++]);
                        // printf("i sent a process\n");
                        // *currentProcessHPF = Processes[i++];
                    }
                }
            }
        }
    }
    // printf("the pointer from origin is = %p\n", PriorityQueueHPF);
    pq_display(PriorityQueueHPF);
    // printf("end of process generator\n");
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 7. Clear clock resources
    // destroyClk(true);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    killpg(getpgrp(), SIGINT);
    exit(0);
}