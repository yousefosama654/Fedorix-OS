#include "headers.h"
#include "priority_queue.h"
#include "circular_queue.h"
PriorityQueue PriorityQueueHPF;
PriorityQueue PriorityQueueSRTF;
CircularQueue CircularQueueRR;
int prevclk = -1;
int Quantum;
PCB *processtable;
int FinishedProcessesNO = 0;
int ProcessesNum;
int laststart; // start of last quantum

PCB *RunningProcess = NULL;
PCB currentProcess;

void SignalINThandler(int signnum);
void terminateprocess(int signnum);
void StartProcess(PCB *process);
void ResumeProcess(PCB *process);
void PauseProcess(PCB *process);
void Calc();
void CalculateFinalState();
void HPF()
{

    if (RunningProcess == NULL && pq_empty(&PriorityQueueHPF) == false)
    {
        currentProcess = pq_pop(&PriorityQueueHPF);
        RunningProcess = &currentProcess;
        StartProcess(&currentProcess);
    }
}
void SRTN()
{

    if (RunningProcess == NULL)
    {
        if (pq_empty(&PriorityQueueSRTF) == false)
        {
            RunningProcess = (PCB *)malloc(sizeof(PCB));

            *RunningProcess = pq_pop(&PriorityQueueSRTF);
            if (processtable[RunningProcess->id - 1].cummultiverunningtime == 0)
            {

                StartProcess(RunningProcess);
            }
            else
            {

                ResumeProcess(RunningProcess);
            }
        }
    }
    else
    {
        if (pq_empty(&PriorityQueueSRTF) == 0)
        {
            PCB firstp = peek(&PriorityQueueSRTF);
            //  printf("\n%d is the next\n", RunningProcess->id);
            if (processtable[firstp.id - 1].runningtime - processtable[firstp.id - 1].cummultiverunningtime < processtable[RunningProcess->id - 1].runningtime - processtable[RunningProcess->id - 1].cummultiverunningtime)
            {

                // printf("\n%d %d\n", firstp.runningtime - firstp.cummultiverunningtime, RunningProcess->runningtime - RunningProcess->cummultiverunningtime);
                PauseProcess(RunningProcess);
                pq_pushSRTN(&PriorityQueueSRTF, *RunningProcess);
                *RunningProcess = pq_pop(&PriorityQueueSRTF);
                if (processtable[RunningProcess->id - 1].cummultiverunningtime == 0)
                {

                    StartProcess(RunningProcess);
                }
                else
                {

                    ResumeProcess(RunningProcess);
                }
            }
        }
    }
}

void RR()
{
    if (RunningProcess == NULL)
    {
        if (q_empty(&CircularQueueRR) == false)
        {
            RunningProcess = (PCB *)malloc(sizeof(PCB));
            *RunningProcess = q_pop(&CircularQueueRR);
            if (processtable[RunningProcess->id - 1].cummultiverunningtime == 0)
            {
                StartProcess(RunningProcess);
                laststart = getClk();
            }
            else
            {
                ResumeProcess(RunningProcess);
                laststart = getClk();
            }
        }
    }
    else if (getClk() - laststart == Quantum)
    {
        if (processtable[RunningProcess->id - 1].cummultiverunningtime != processtable[RunningProcess->id - 1].runningtime && strcmp(processtable[RunningProcess->id - 1].state, "finished") != 0)
        {
            PauseProcess(RunningProcess);
            q_push(&CircularQueueRR, *RunningProcess);
            if (q_empty(&CircularQueueRR) == false)
            {
                *RunningProcess = q_pop(&CircularQueueRR);
                if (processtable[RunningProcess->id - 1].cummultiverunningtime == 0)
                {
                    StartProcess(RunningProcess);
                    laststart = getClk();
                }
                else
                {
                    ResumeProcess(RunningProcess);
                    laststart = getClk();
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, SignalINThandler);
    signal(SIGUSR1, terminateprocess);
    initClk();
    char *Scheduling_Algo = argv[1];
    ProcessesNum = atoi(argv[2]);
    Quantum = atoi(argv[3]);

    processtable = (PCB *)malloc(sizeof(PCB) * ProcessesNum);
    // initilize ids with -1
    for (int i = 0; i < ProcessesNum; i++)
    {
        processtable[i].id = -1;
        processtable[i].cummultiverunningtime = 0;
    }

    int lasttaken = -1; // last element taken from shared memory
    int msgid = msgget(MKEY, IPC_CREAT | 0666);
    ProcessMessage PSM;
    if (strcmp(Scheduling_Algo, "RR") == 0)
    {
        q_init(&CircularQueueRR);
        printf("Round Robin Algorithm starting...\n");
        int lastprevClk = getClk();
        while (1)
        {
            int msg = msgrcv(msgid, &PSM, sizeof(PSM.p), 0, IPC_NOWAIT);
            prevclk = getClk();
            int previd = -1;
            while (msg != -1 && prevclk == getClk() && previd != PSM.p.id)
            {
                PCB TempProcess;
                TempProcess.id = PSM.p.id;
                TempProcess.arrivaltime = PSM.p.arrivaltime;
                TempProcess.priority = PSM.p.priority;
                TempProcess.runningtime = PSM.p.runningtime;
                processtable[PSM.p.id - 1].runningtime = PSM.p.runningtime;
                processtable[PSM.p.id - 1].arrivaltime = PSM.p.arrivaltime;
                processtable[PSM.p.id - 1].id = PSM.p.id;
                strcpy(processtable[PSM.p.id - 1].state, "waiting");
                q_push(&CircularQueueRR, TempProcess);
                previd = PSM.p.id;
                msg = msgrcv(msgid, &PSM, sizeof(PSM.p), 0, IPC_NOWAIT);
            }
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

    else if (strcmp(Scheduling_Algo, "SRTN") == 0)
    {
        printf("Shortest Remaining Time Next Algorithm starting...\n");
        int lastprevClk = getClk();
        pq_init(&PriorityQueueSRTF);
        while (1)
        {
            int msg = msgrcv(msgid, &PSM, sizeof(PSM.p), 0, IPC_NOWAIT);
            prevclk = getClk();
            int previd = -1;

            while (msg != -1 && prevclk == getClk() && previd != PSM.p.id)
            {
                PCB TempProcess;
                TempProcess.id = PSM.p.id;
                TempProcess.arrivaltime = PSM.p.arrivaltime;
                TempProcess.priority = PSM.p.priority;
                TempProcess.runningtime = PSM.p.runningtime;

                processtable[PSM.p.id - 1].runningtime = PSM.p.runningtime;
                processtable[PSM.p.id - 1].arrivaltime = PSM.p.arrivaltime;
                processtable[PSM.p.id - 1].id = PSM.p.id;
                strcpy(processtable[PSM.p.id - 1].state, "waiting");
                pq_pushSRTN(&PriorityQueueSRTF, TempProcess);
                previd = PSM.p.id;
                msg = msgrcv(msgid, &PSM, sizeof(PSM.p), 0, IPC_NOWAIT);
            }

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
        int lastprevClk = getClk();
        printf("Highest Prioity First Next Algorithm starting...\n");
        while (1)
        {
            int msg = msgrcv(msgid, &PSM, sizeof(PSM.p), 0, IPC_NOWAIT);
            prevclk = getClk();
            int previd = -1;
            while (msg != -1 && prevclk == getClk() && previd != PSM.p.id)
            {
                PCB TempProcess;
                TempProcess.id = PSM.p.id;
                TempProcess.arrivaltime = PSM.p.arrivaltime;

                TempProcess.priority = PSM.p.priority;
                TempProcess.runningtime = PSM.p.runningtime;
                pq_push(&PriorityQueueHPF, TempProcess);
                previd = PSM.p.id;

                processtable[PSM.p.id - 1].runningtime = PSM.p.runningtime;
                processtable[PSM.p.id - 1].arrivaltime = PSM.p.arrivaltime;
                strcpy(processtable[PSM.p.id - 1].state, "waiting");
                msg = msgrcv(msgid, &PSM, sizeof(PSM.p), 0, IPC_NOWAIT);
            }
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
    free(processtable);
    kill(getppid(), SIGINT);
    CalculateFinalState();
}
// TODO implement the scheduler :)
// upon termination release the clock resources.

void SignalINThandler(int signnum)
{
    free(processtable);
    kill(getppid(), SIGINT);
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
    fprintf(pFile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f.\n", getClk(), processtable[RunningProcess->id - 1].id, processtable[RunningProcess->id - 1].arrivaltime, processtable[RunningProcess->id - 1].runningtime, processtable[RunningProcess->id - 1].waitingtime, getClk() - processtable[RunningProcess->id - 1].arrivaltime, (getClk() - processtable[RunningProcess->id - 1].arrivaltime) / (float)processtable[RunningProcess->id - 1].runningtime);
    fclose(pFile);
    RunningProcess = NULL;
    //

    signal(SIGUSR1, terminateprocess);
}

void StartProcess(PCB *process)
{
    // fill the pcb for each process at the start of it
    processtable[process->id - 1].waitingtime = getClk() - process->arrivaltime;
    processtable[process->id - 1].id = process->id;

    processtable[process->id - 1].starttime = getClk();
    processtable[process->id - 1].responsetime = getClk() - process->arrivaltime;
    processtable[process->id - 1].priority = getClk() - process->priority;
    strcpy(processtable[process->id - 1].state, "running");
    printf("\nprocess %d has started in clock %d\n", RunningProcess->id, getClk());
    int pid = fork();

    if (pid == 0)
    {
        execl("./bin/process.out", "process.out", tostring(process->runningtime), NULL);
    }
    else
    {
        processtable[process->id - 1].Pid = pid;
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
    fprintf(pFile, "At time %d process %d started arr %d total %d remain %d wait %d.\n", getClk(), processtable[process->id - 1].id, processtable[process->id - 1].arrivaltime, processtable[process->id - 1].runningtime, processtable[process->id - 1].runningtime - processtable[process->id - 1].cummultiverunningtime, -processtable[process->id - 1].arrivaltime + processtable[process->id - 1].starttime);
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
            }

            if (strcmp(processtable[i].state, "stopped") == 0 || strcmp(processtable[i].state, "waiting") == 0) // if process is stopped increment the waiting time
            {
                processtable[i].waitingtime += 1;
            }
        }
    }
}

void ResumeProcess(PCB *process)
{
    strcpy(processtable[process->id - 1].state, "running");
    printf("\nprocess %d has resumed in clock %d\n", processtable[process->id - 1].id, getClk());
    int pid = processtable[process->id - 1].Pid;
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
    fprintf(pFile, "At time %d process %d  stopped arr %d total %d remain %d wait %d.\n", getClk(), processtable[process->id - 1].id, processtable[process->id - 1].arrivaltime, processtable[process->id - 1].runningtime, processtable[process->id - 1].runningtime - processtable[process->id - 1].cummultiverunningtime, processtable[process->id - 1].waitingtime);
    fclose(pFile);
}

void PauseProcess(PCB *process)
{
    int pid = processtable[process->id - 1].Pid;
    strcpy(processtable[process->id - 1].state, "stopped");
     kill(pid, SIGUSR1);
    kill(pid, SIGSTOP);
    printf("\nprocess %d has paused in clock %d \n", processtable[process->id - 1].id, getClk());
    // print in output file
    FILE *pFile;
    // 1. Read the input files.
    pFile = fopen("./Log/Scheduler.log", "a");
    if (pFile == NULL)
    {
        printf("No such file Scheduler.log in Log Directory\n");
        return;
    }
    fprintf(pFile, "At time %d process %d  resumed arr %d total %d remain %d wait %d.\n", getClk(), processtable[process->id - 1].id, processtable[process->id - 1].arrivaltime, processtable[process->id - 1].runningtime, processtable[process->id - 1].runningtime - processtable[process->id - 1].cummultiverunningtime, processtable[process->id - 1].waitingtime);
    fclose(pFile);
}
void CalculateFinalState()
{
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
    pFile = fopen("./Log/Scheduler.perf", "a");
    if (pFile == NULL)
    {
        printf("No such file Scheduler.log in Log Directory\n");
        return;
    }
    for (int i = 0; i < ProcessesNum; i++)
    {
        TA += processtable[i].finishtime - processtable[i].arrivaltime;
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
        float x = (y - AvgWTA) * (y - AvgWTA);
        StdWTA += x;
    }
    StdWTA /= (float)(ProcessesNum - 1);
    StdWTA = (float)sqrt((double)StdWTA);
    fprintf(pFile, "CPU utilization= %.2f %%\n", utilization);
    fprintf(pFile, "Avg WTA = %.2f\n", AvgWTA);
    fprintf(pFile, "Avg Waiting = %.2f\n", AvgWaiting);
    fprintf(pFile, "Std WTA = %.2f\n", StdWTA);
    fclose(pFile);
}