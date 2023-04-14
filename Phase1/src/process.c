#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int flag = 0;

void SIGUSER1Handler(int signumber)
{
    flag = 1;
}

int main(int agrc, char *argv[])
{
    signal(SIGUSR1, SIGUSER1Handler);
    initClk();
    // TODO it needs to get the remaining time from somewhere
    remainingtime = atoi(argv[1]);

    int prev = getClk();
    while (remainingtime > 0)
    {
        if (getClk() - prev == 1 && flag == 0)
        {
            remainingtime--;
            prev = getClk();
        }
        else if (getClk() - prev > 1 || flag == 1)
        {
            prev = getClk();
        }
        flag = 0;
    }
    kill(getppid(), SIGUSR1);
    destroyClk(false);
    return 0;
}
