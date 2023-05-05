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
    char *filename = argv[1];
    int Memory_Algo;
    int Scheduling_Algo = atoi(argv[3]);
    if (Scheduling_Algo != 1 && Scheduling_Algo != 2 && Scheduling_Algo != 3)
    {
        printf("Fedorix OS Schdeuler does not supprt this Algorithm.\n");
        return 0;
    }
    if (Scheduling_Algo == 3)
    {
        Quantum = atoi(argv[5]);
        Memory_Algo = atoi(argv[7]);
    }
    else
    {
        Memory_Algo = atoi(argv[5]);
    }
    if (Memory_Algo != 1 && Memory_Algo != 2)
    {
        printf("Fedorix OS Schdeuler does not supprt this memory managment algorithm.\n");
        return 0;
    }

    FILE *pFile;
    // 1. Read the input files.
    char file[30] = "./Log/";
    strcat(file, filename);
    pFile = fopen(file, "r");
    if (pFile == NULL)
    {
        printf("No such file processes.txt in Log Directory\n");
        return 0;
    }
    char firstline[100];
    fgets(firstline, sizeof(firstline), pFile);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    // printf("Please choose the appropraite scheduling algorithm\n");
    // printf("for Highest priority first->1,for Shortest remaining time next-> 2,for Round Robin-> 3 \n ");
    fscanf(pFile, "%d", &ProcessesNum);
    processData *Processes = (processData *)malloc(ProcessesNum * sizeof(processData));
    for (int i = 0; i < ProcessesNum; i++)
    {
        fscanf(pFile, "%d", &Processes[i].id);
        fscanf(pFile, "%d", &Processes[i].arrivaltime);
        fscanf(pFile, "%d", &Processes[i].runningtime);
        fscanf(pFile, "%d", &Processes[i].priority);
        fscanf(pFile, "%d", &Processes[i].memsize);
    }
    // 3. Initiate and create the scheduler and clock processes.
    int pid = fork();
    if (pid == 0)
    {
        execl("./bin/scheduler.out", "scheduler.out", tostring(Scheduling_Algo), tostring(ProcessesNum), tostring(Quantum), tostring(Memory_Algo), NULL);
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
            msgid = msgget(MKEY, IPC_CREAT | 0666);
            ProcessMessage PSM;
            while (true)
            {
                // 4. Use this function after creating the clock process to initialize clock
                // To get time use this function
                int x = getClk();
                if (i != ProcessesNum && Processes[i].arrivaltime == x) // sent process to scheduler when it arrives
                {

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