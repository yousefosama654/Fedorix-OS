#include "headers.h"
#include "priority_queue.h"
void clearResources(int);
PriorityQueue *getPrioityQueueHPF()
{
    // Create shared memory for one integer variable 4 bytes
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
PriorityQueue *PriorityQueueHPF = NULL;
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
            while (1)
            {
                // 4. Use this function after creating the clock process to initialize clock
                initClk();
                // To get time use this function
                int x = getClk();
                // printf("current time is %d\n", x);
                if (Processes[i].arrivaltime == x)
                {
                    // 6. Send the information to the scheduler at the appropriate time.
                    if (strcmp(Scheduling_Algo, "RR") == 0)
                    {
                        PriorityQueueHPF = getPrioityQueueHPF();
                        pq_push(PriorityQueueHPF, Processes[i]);
                        i++;
                    }
                }
            }
        }
    }
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    killpg(getpgrp(), SIGINT);
    exit(0);
}