#include "headers.h"
#include "priority_queue.h"
#include "circular_queue.h"
#include "priority_queue_int.h"
PriorityQueue PriorityQueueHPF;
PriorityQueue PriorityQueueSRTF;
CircularQueue CircularQueueRR;
int prevclk = -1;
int Quantum;
PCB *processtable;
int FinishedProcessesNO = 0;
int ProcessesNum;
int laststart;       // start of last quantum
int Scheduling_Algo; // HPF or RR or SRTN
int **addresses;     // addreses of shared memory with each process to calc Remaining time
int *sharedids;
int lastprevClk;
int msgid; // id of message queue
PCB *RunningProcess = NULL;
void SignalINThandler(int signnum);
void terminateprocess(int signnum);
void StartProcess();
void ResumeProcess();
void PauseProcess();
void startFiles();
void Calc();
void freeall();
void CalculateFinalState();
void fillProcessTable(processData p);
void HPF();
void SRTN();
void RR();
void recieve();
int main(int argc, char *argv[])
{
    signal(SIGINT, SignalINThandler);
    signal(SIGUSR1, terminateprocess);

    startFiles();
    initClk();

    Scheduling_Algo = atoi(argv[1]);
    ProcessesNum = atoi(argv[2]);
    addresses = (int **)malloc(sizeof(int *) * ProcessesNum);
    sharedids = (int *)malloc(sizeof(int) * ProcessesNum);
    Quantum = atoi(argv[3]);
    processtable = (PCB *)malloc(sizeof(PCB) * ProcessesNum);
    // initilize ids with -1
    for (int i = 0; i < ProcessesNum; i++)
    {
        processtable[i].id = -1;
        processtable[i].cummultiverunningtime = 0;
    }
    msgid = msgget(MKEY, IPC_CREAT | 0666);
    if (Scheduling_Algo == 3)
    {
        q_init(&CircularQueueRR);
        printf("Round Robin Algorithm starting...\n");
        lastprevClk = getClk();
        while (1)
        {
            recieve();
            if (getClk() != lastprevClk)
            {
                Calc();
                lastprevClk = getClk();
            }
            RR();
            if (ProcessesNum == FinishedProcessesNO)
                break;
        }
    }

    else if (Scheduling_Algo == 2)
    {
        printf("Shortest Remaining Time Next Algorithm starting...\n");
        lastprevClk = getClk();
        pq_init(&PriorityQueueSRTF);
        while (1)
        {
            recieve();
            if (getClk() != lastprevClk)
            {
                Calc();
                lastprevClk = getClk();
            }
            SRTN();
            if (ProcessesNum == FinishedProcessesNO)
                break;
        }
    }
    else
    {
        lastprevClk = getClk();
        printf("Highest Prioity First Algorithm starting...\n");
        while (1)
        {
            recieve();
            if (lastprevClk != getClk())
            {
                Calc();
                lastprevClk = getClk();
            }
            HPF();
            if (ProcessesNum == FinishedProcessesNO)
                break;
        }
    }
    CalculateFinalState();
    freeall();
    kill(getppid(), SIGINT);
}
void freeall()
{
    free(processtable);
    for (int i = 0; i < ProcessesNum; i++)
        shmctl(sharedids[i], IPC_RMID, (struct shmid_ds *)0);

    free(addresses);
    free(sharedids);
}
void SignalINThandler(int signnum)
{
    free(processtable);
    kill(getppid(), SIGINT);
    free(addresses);
    free(sharedids);
    exit(0);
}
void terminateprocess(int signnum)
{

    FinishedProcessesNO++;
    strcpy(processtable[RunningProcess->id - 1].state, "finished");
    processtable[RunningProcess->id - 1].finishtime = getClk();
    printf("\nprocess %d has finished in clock %d\n", RunningProcess->id, getClk());
    // print in output file
    FILE *pFile;
    // 1. Read the input files.
    pFile = fopen("./Log/Scheduler.log", "a");
    if (pFile == NULL)
    {
        printf("No such file Scheduler.log in Log Directory\n");
        return;
    }
    int TA = getClk() - processtable[RunningProcess->id - 1].arrivaltime;
    float WTA = 0;
    if (processtable[RunningProcess->id - 1].runningtime != 0)
    {
        WTA = (getClk() - processtable[RunningProcess->id - 1].arrivaltime) / (float)processtable[RunningProcess->id - 1].runningtime;
    }

    fprintf(pFile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f.\n", getClk(), processtable[RunningProcess->id - 1].id, processtable[RunningProcess->id - 1].arrivaltime, processtable[RunningProcess->id - 1].runningtime, processtable[RunningProcess->id - 1].waitingtime, TA, WTA);
    fclose(pFile);

    RunningProcess = NULL;

    signal(SIGUSR1, terminateprocess);
    /// search if there is another process can be allocated in the freed memory from the waiting list
}
void StartProcess()
{
    // fill the pcb for each process at the start of it
    processtable[RunningProcess->id - 1].waitingtime = laststart - RunningProcess->arrivaltime;
    processtable[RunningProcess->id - 1].id = RunningProcess->id;
    processtable[RunningProcess->id - 1].starttime = laststart;
    processtable[RunningProcess->id - 1].responsetime = laststart - RunningProcess->arrivaltime;
    processtable[RunningProcess->id - 1].priority = laststart - RunningProcess->priority;
    strcpy(processtable[RunningProcess->id - 1].state, "running");
    printf("\nprocess %d has started in clock %d\n", RunningProcess->id, laststart);
    int shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0644);
    void *shmaddr = shmat(shmid, (void *)0, 0);
    addresses[RunningProcess->id - 1] = (int *)shmaddr;
    sharedids[RunningProcess->id - 1] = shmid;
    strcpy((char *)shmaddr, tostring(RunningProcess->runningtime));
    int pid = fork();
    if (pid == 0)
    {
        execl("./bin/process.out", "process.out", tostring(shmid), NULL);
    }
    else
    {
        processtable[RunningProcess->id - 1].Pid = pid;
    }
    // print in output file
    FILE *pFile;
    // 1. Read the input files.
    pFile = fopen("./Log/Scheduler.log", "a");
    if (pFile == NULL)
    {
        printf("No such file Scheduler.log in Log Directory\n");
        return;
    }

    fprintf(pFile, "At time %d process %d started arr %d total %d remain %d wait %d.\n", laststart, processtable[RunningProcess->id - 1].id, processtable[RunningProcess->id - 1].arrivaltime, processtable[RunningProcess->id - 1].runningtime, processtable[RunningProcess->id - 1].runningtime - processtable[RunningProcess->id - 1].cummultiverunningtime, -processtable[RunningProcess->id - 1].arrivaltime + processtable[RunningProcess->id - 1].starttime);
    fclose(pFile);
}
void Calc()
{

    for (int i = 0; i < ProcessesNum; i++)
    {
        if (processtable[i].id != -1)
        {
            if (strcmp(processtable[i].state, "running") == 0) // if process is running increment the cummulitive running time
            {

                processtable[i].cummultiverunningtime += 1;
                strcpy((char *)addresses[i], tostring(processtable[i].runningtime - processtable[i].cummultiverunningtime));
            }

            if (strcmp(processtable[i].state, "stopped") == 0 || strcmp(processtable[i].state, "waiting") == 0) // if process is stopped increment the waiting time
            {
                processtable[i].waitingtime += 1;
            }
        }
    }
}
void ResumeProcess()
{
    // pop the proccess from the ready queue and run it again
    strcpy(processtable[RunningProcess->id - 1].state, "running");
    printf("\nprocess %d has resumed in clock %d\n", processtable[RunningProcess->id - 1].id, laststart);
    int pid = processtable[RunningProcess->id - 1].Pid;
    kill(pid, SIGCONT);
    // print in output file
    FILE *pFile;
    // 1. Read the input files.
    pFile = fopen("./Log/Scheduler.log", "a");
    if (pFile == NULL)
    {
        printf("No such file Scheduler.log in Log Directory\n");
        return;
    }
    fprintf(pFile, "At time %d process %d  resumed arr %d total %d remain %d wait %d.\n", laststart, processtable[RunningProcess->id - 1].id, processtable[RunningProcess->id - 1].arrivaltime, processtable[RunningProcess->id - 1].runningtime, processtable[RunningProcess->id - 1].runningtime - processtable[RunningProcess->id - 1].cummultiverunningtime, processtable[RunningProcess->id - 1].waitingtime);
    fclose(pFile);
}
void PauseProcess()
{
    // pause the running proccess and push it in the ready queue when it is preemted
    sleep(0.01);
    recieve();
    int pid = processtable[RunningProcess->id - 1].Pid;
    strcpy(processtable[RunningProcess->id - 1].state, "stopped");
    kill(pid, SIGSTOP);
    printf("\nprocess %d has paused in clock %d \n", processtable[RunningProcess->id - 1].id, laststart);
    // print in output file
    FILE *pFile;
    // 1. Read the input files.
    pFile = fopen("./Log/Scheduler.log", "a");
    if (pFile == NULL)
    {
        printf("No such file Scheduler.log in Log Directory\n");
        return;
    }
    fprintf(pFile, "At time %d process %d  stopped arr %d total %d remain %d wait %d.\n", laststart, processtable[RunningProcess->id - 1].id, processtable[RunningProcess->id - 1].arrivaltime, processtable[RunningProcess->id - 1].runningtime, processtable[RunningProcess->id - 1].runningtime - processtable[RunningProcess->id - 1].cummultiverunningtime, processtable[RunningProcess->id - 1].waitingtime);
    fclose(pFile);
}
void CalculateFinalState()
{
    // calculations for schadular.pref
    float AvgWTA = 0;
    float AvgWaiting = 0;
    float StdWTA = 0;
    float TA = 0;
    float WTA = 0;
    float waiting = 0;
    float totalrunningTime = 0;
    float utilization = 0;
    // print in output file
    FILE *pFile;
    // 1. Read the input files.
    pFile = fopen("./Log/Scheduler.perf", "w");
    if (pFile == NULL)
    {
        printf("No such file Scheduler.log in Log Directory\n");
        return;
    }
    for (int i = 0; i < ProcessesNum; i++)
    {
        TA += processtable[i].finishtime - processtable[i].arrivaltime;
        if (processtable[i].runningtime)
            WTA += (float)(processtable[i].finishtime - processtable[i].arrivaltime) / (float)processtable[i].runningtime;
        waiting += processtable[i].waitingtime;
        totalrunningTime += processtable[i].runningtime;
    }

    AvgWTA = WTA / (float)ProcessesNum;
    AvgWaiting = waiting / ProcessesNum;
    utilization = (totalrunningTime / getClk()) * 100;

    for (int i = 0; i < ProcessesNum; i++)
    {
        float y = (float)(processtable[i].finishtime - processtable[i].arrivaltime) / (float)processtable[i].runningtime;
        if (processtable[i].runningtime == 0)
            y = 0;
        float x = (y - AvgWTA) * (y - AvgWTA);
        StdWTA += x;
    }
    StdWTA /= (float)(ProcessesNum - 1);
    StdWTA = (float)sqrt((double)StdWTA);
    fprintf(pFile, "CPU utilization = %.2f %%\n", utilization);
    fprintf(pFile, "Avg WTA = %.2f\n", AvgWTA);
    fprintf(pFile, "Avg Waiting = %.2f\n", AvgWaiting);
    fprintf(pFile, "Std WTA = %.2f\n", StdWTA);
    fclose(pFile);
}
void fillProcessTable(processData p)
{
    // after recieving the proccess from proccess generator fill its information in PCB
    processtable[p.id - 1].runningtime = p.runningtime;
    processtable[p.id - 1].arrivaltime = p.arrivaltime;
    processtable[p.id - 1].priority = p.priority;
    processtable[p.id - 1].id = p.id;
    strcpy(processtable[p.id - 1].state, "waiting");
}
void startFiles()
{ // start files with needed comments
    FILE *pFile;
    // 1. Read the input files.
    pFile = fopen("./Log/Scheduler.log", "a");
    fprintf(pFile, "#At time x process y state arr w total z remain y wait k.\n");
    fclose(pFile);
}

void RR()
{
    // if no process is running choose proccess in the top of the queue
    if (RunningProcess == NULL)
    {
        laststart = getClk();
        if (q_empty(&CircularQueueRR) == false)
        {
            RunningProcess = (PCB *)malloc(sizeof(PCB));
            *RunningProcess = q_pop(&CircularQueueRR);
            if (processtable[RunningProcess->id - 1].cummultiverunningtime == 0)
            {
                StartProcess();
            }
            else
            {
                ResumeProcess();
            }
        }
    }
    // if the running proccess's quantm finished pause and start or resume the next one
    else if (getClk() - laststart >= Quantum)
    {
        laststart = getClk();
        if (processtable[RunningProcess->id - 1].cummultiverunningtime < processtable[RunningProcess->id - 1].runningtime && strcmp(processtable[RunningProcess->id - 1].state, "finished") != 0)
        {
            if (getClk() != lastprevClk)
            {
                Calc();
                lastprevClk = getClk();
            }
            PauseProcess();
            q_push(&CircularQueueRR, *RunningProcess);
            if (q_empty(&CircularQueueRR) == false)
            {
                *RunningProcess = q_pop(&CircularQueueRR);
                if (processtable[RunningProcess->id - 1].cummultiverunningtime == 0)
                {
                    StartProcess();
                }
                else
                {
                    ResumeProcess();
                }
            }
        }
    }
}
void HPF()
{ // schadule the highest priority (lowest value)

    if (RunningProcess == NULL && pq_empty(&PriorityQueueHPF) == false)
    {
        RunningProcess = (PCB *)malloc(sizeof(PCB));

        *RunningProcess = pq_pop(&PriorityQueueHPF);
        laststart = getClk();
        StartProcess();
    }
}
void SRTN()
{
    // if no process is running choose the least remainnig time process to run
    if (RunningProcess == NULL)
    {
        if (pq_empty(&PriorityQueueSRTF) == false)
        {
            laststart = getClk();
            RunningProcess = (PCB *)malloc(sizeof(PCB));

            *RunningProcess = pq_pop(&PriorityQueueSRTF);
            if (processtable[RunningProcess->id - 1].cummultiverunningtime == 0)
            {

                StartProcess();
            }
            else
            {

                ResumeProcess();
            }
        }
    }
    else
    { // if the remaining time of any proccees is less than the remaining time of running one pause it and start or resume the other process
        if (pq_empty(&PriorityQueueSRTF) == 0)
        {
            laststart = getClk();
            PCB firstp = peek(&PriorityQueueSRTF);
            if (processtable[firstp.id - 1].runningtime - processtable[firstp.id - 1].cummultiverunningtime < processtable[RunningProcess->id - 1].runningtime - processtable[RunningProcess->id - 1].cummultiverunningtime)
            {

                PauseProcess();
                *RunningProcess = processtable[RunningProcess->id - 1];
                pq_pushSRTN(&PriorityQueueSRTF, *RunningProcess);
                *RunningProcess = pq_pop(&PriorityQueueSRTF);
                if (processtable[RunningProcess->id - 1].cummultiverunningtime == 0)
                {

                    StartProcess();
                }
                else
                {

                    ResumeProcess();
                }
            }
        }
    }
}
void recieve() // recive processes from process generator using message queue when process arrive
{
    ProcessMessage PSM;

    int msg = msgrcv(msgid, &PSM, sizeof(PSM.p), 0, IPC_NOWAIT);
    prevclk = getClk();
    int previd = -1;
    ;
    while (msg != -1 && prevclk == getClk() && previd != PSM.p.id)
    {

        fillProcessTable(PSM.p);
        if (Scheduling_Algo == 3)
            q_push(&CircularQueueRR, processtable[PSM.p.id - 1]);
        else if (Scheduling_Algo == 2)
            pq_pushSRTN(&PriorityQueueSRTF, processtable[PSM.p.id - 1]);
        else if (Scheduling_Algo == 1)
            pq_push(&PriorityQueueHPF, processtable[PSM.p.id - 1]);

        previd = PSM.p.id;
        msg = msgrcv(msgid, &PSM, sizeof(PSM.p), 0, IPC_NOWAIT);
    }
}