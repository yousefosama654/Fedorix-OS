#include "headers.h"

/* Modify this file as needed*/
int remainingtime;


int main(int agrc, char *argv[])
{
    initClk();

    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
int prev=getClk();
    while (remainingtime > 0)
    {

        if(getClk()-1==prev)
        remainingtime --;

        prev=getClk();


    }
    kill(getppid(),SIGUSR1);

    destroyClk(false);

    return 0;
}
