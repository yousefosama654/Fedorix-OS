#ifndef HEADERS_H_
#define HEADERS_H_
#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "string.h"
typedef short bool;
typedef struct
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
} processData;
typedef struct
{
   
    int id;
    int Pid;
    char state[10];
    int priority;
    int arrivaltime;
    int runningtime;
    int waitingtime;

    int starttime;
    int responsetime;
    int cummultiverunningtime;   
   

} PCB;
#define true 1
#define false 0

#define SHKEY 300

#define PKEY 200

///==============================
// don't mess with this variable//
int *shmaddr; //
//===============================

int getClk()
{
    return *shmaddr;
}

/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
 */
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        // Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
    
          sleep(1);
        
              
        shmid = shmget(SHKEY, 4, 0444);
    }
   
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
   
}

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
 */

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
#endif /* HEADERS_H_ */
