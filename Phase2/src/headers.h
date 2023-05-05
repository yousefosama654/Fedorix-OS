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
#include "math.h"
typedef short bool;
typedef struct
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    int memsize;
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
    int finishtime;
    int starttime;
    int responsetime;
    int cummultiverunningtime;
    int memsize;
    int memidx;
    int memblocks;
} PCB;
typedef struct
{
    processData p;
    long mtype;
} ProcessMessage;

#define true 1
#define false 0

#define MKEY 200

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

char *tostring(int num)
{
    char *str = (char *)malloc(12); // Allocate enough memory for the largest possible integer (10 digits) plus a null terminator
    snprintf(str, 12, "%d", num);   // Convert the integer to a string using snprintf()
    return str;
}

#endif /* HEADERS_H_ */
