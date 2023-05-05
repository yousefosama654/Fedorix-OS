#include "headers.h"


int remainingtime;

int main(int agrc, char *argv[])
{
    
    initClk();
    
    int shmid = atoi(argv[1]);
    void *shmaddr = shmat(shmid, (void *)0, 0);
    remainingtime= atoi((char *)shmaddr);
   
    while (remainingtime > 0)//get remaining time from shared memory with scheduler
    {

        remainingtime= atoi((char *)shmaddr);
    }
    kill(getppid(), SIGUSR1);
    destroyClk(false);
    return 0;
}
