#include "headers.h"
#include "priority_queue.h"
#include "circular_queue.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
void clearResources(int);
int ProcessesNum;
int Quantum = 0;
int msgid;

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    char *Scheduling_Algo;
    FILE *pFile;
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
    processData *Processes = (processData *)malloc(ProcessesNum * sizeof(processData));
    for (int i = 0; i < ProcessesNum; i++)
    {
        fscanf(pFile, "%d", &Processes[i].id);
        fscanf(pFile, "%d", &Processes[i].arrivaltime);
        fscanf(pFile, "%d", &Processes[i].runningtime);
        fscanf(pFile, "%d", &Processes[i].priority);
        // Processes[i].waitingtime = 0;
    }
    scanf("%ms", &Scheduling_Algo);
    if (strcmp(Scheduling_Algo, "SRTN") != 0 && strcmp(Scheduling_Algo, "HPF") != 0 && strcmp(Scheduling_Algo, "RR") != 0)
    {
        printf("Fedorix OS Schdeuler does not supprt this Algorithm.\n");
        return 0;
    }
    if (strcmp(Scheduling_Algo, "RR") == 0)
    {
        printf("Enter Quantum:\n");
        scanf("%d", &Quantum);
    }
    // 3. Initiate and create the scheduler and clock processes.
    int pid = fork();
    if (pid == 0)
    {
        execl("./bin/scheduler.out", "scheduler.out", Scheduling_Algo, tostring(ProcessesNum), tostring(Quantum), NULL);
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
            // must be terminated
            while (true)
            {
                // 4. Use this function after creating the clock process to initialize clock
                // To get time use this function
                int x = getClk();
                if (i != ProcessesNum && Processes[i].arrivaltime == x)
                {
                    msgid = msgget(MKEY, IPC_CREAT | 0666);
                    ProcessMessage PSM;
                    PSM.p = Processes[i++];
                    PSM.mtype = 1;
                    int msg = msgsnd(msgid, &PSM, sizeof(PSM.p), !IPC_NOWAIT);
                    if (msg == -1)
                    {
                        printf("\nerror in sending process with id = %d\n", PSM.p.id);
                    }
                }
            }
        }
    }
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    msgctl(msgid, IPC_RMID, (struct msqid_ds *)0);
    destroyClk(true);
    exit(0);
}