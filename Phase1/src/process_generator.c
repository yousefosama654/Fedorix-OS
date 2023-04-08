#include "headers.h"
#include "priority_queue.h"
#include "circular_queue.h"
#include<string.h>
#include<stdio.h>
#include <stdlib.h>
void clearResources(int);
  int ProcessesNum;
char* tostring(int num) {
    char* str = (char*)malloc(12);  // Allocate enough memory for the largest possible integer (10 digits) plus a null terminator
    snprintf(str, 12, "%d", num);  // Convert the integer to a string using snprintf()
    return str;
}


processData *GetProcess()
{
    // Create shared memory for one integer variable 4 bytes
    int qid = shmget(PKEY,sizeof(processData), IPC_CREAT | 0644);
    if ((long)qid == -1)
    {
        perror("Error in creating memory!");
        exit(-1);
    }
    processData*qaddr = (processData*)shmat(qid, (void *)0, 0);
    if ((long)qaddr == -1)
    {
        perror("Error in attaching the memory!");
        exit(-1);
    }
    return qaddr;
}
int Quantum=0;


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
   processData*processmem=GetProcess(); 
  
  
   if(strcmp(Scheduling_Algo, "RR") == 0)
   {
     printf("Enter Quantum:\n");
     scanf("%d", &Quantum);
   }
    
    // 3. Initiate and create the scheduler and clock processes.
    int pid = fork();
    if (pid == 0)
    {
        
        execl("./bin/scheduler.out", "scheduler.out",Scheduling_Algo,tostring(ProcessesNum),tostring(Quantum), NULL);
            
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
processData p;
p.id=-1;
         *processmem=p;
            while (true)
            {
                // 4. Use this function after creating the clock process to initialize clock
                // To get time use this function
                
                int x = getClk();
                // printf("cllockk PG :%d\n",getClk());
               

                // printf("current time is %d\n", x);
                if (Processes[i].arrivaltime == x)
                {
                    //6. Send the information to the scheduler at the appropriate time.
                   if(processmem!=NULL)
                     
                      *processmem=Processes[i];
                    
                        i++;
                   
                }
                 if(i==ProcessesNum)
                 break;
            }
            
        }
    }
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 7. Clear clock resources
    while(1);
    destroyClk(true);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    killpg(getpgrp(), SIGINT);
    exit(0);
}