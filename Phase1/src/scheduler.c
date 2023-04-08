#include "headers.h"
#include "priority_queue.h"
#include"circular_queue.h"
PriorityQueue *PriorityQueueHPF = NULL;
 int prevclk=-1;
 int Quantum;
 PCB *processtable;
 int FinishedProcessesNO=0;
 int ProcessesNum;
 int laststart;//start of last quantum
 processData*RunningProcess=NULL;

CircularQueue CircularQueueRR ; 

void SignalINThandler(int signnum);
void terminateprocess(int signnum);
void StartProcess(processData *process);
void ResumeProcess(processData *process);
void PauseProcess(processData *process);
void Calc();


processData *GetProcess()
{
    // Create shared memory for one integer variable 4 bytes
    int qid = shmget(PKEY,sizeof(processData),   0644);
    if ((long)qid == -1)
    {
        perror("Error in creating memory!");
        exit(-1);
    }
    processData*qaddr = (processData*)shmat(qid, (void *)0, 0);
    if ((long)qaddr == -1)
    {
        perror("Error in attaching the memory!!");
        exit(-1);
    }
    return qaddr;
}

void HPF()
{
    while (pq_empty(PriorityQueueHPF) != false)
    {
        // PCB process = pq_pop(PriorityQueueHPF);
        // printf("pid id %d has arrived safely\n", process.id);
        // int pid = fork();
        // if (pid == 0)
        // {
        //     execl("./bin/process.out", "process.out", NULL);
        // }
        // else
        // {

        // }
    }
}
void SRTN()
{
}

 void RR()
{
   
    Calc();
    
   if(RunningProcess==NULL)
      
   { 
    
       
      if(q_empty(&CircularQueueRR)==false)
      {
                              
                        RunningProcess=(processData*)malloc(sizeof(processData));
                        *RunningProcess=q_pop(&CircularQueueRR);
                         
                        if(processtable[RunningProcess->id-1].cummultiverunningtime==0)
                         {    printf("\nfds\n")  ;  
                            StartProcess(RunningProcess);
                             laststart=getClk();
                         }
                        else{
                            ResumeProcess(RunningProcess);
                             laststart=getClk();
                        }
            }
   }
   else if(getClk()-laststart==Quantum)
   {
        PauseProcess(RunningProcess);
        q_push(&CircularQueueRR,*RunningProcess);
        *RunningProcess=q_pop(&CircularQueueRR);
        if(processtable[RunningProcess->id-1].cummultiverunningtime==0)
        { StartProcess(RunningProcess);
                laststart=getClk();
        }
        else{
            ResumeProcess(RunningProcess);
                laststart=getClk();
        }
    
   }


prevclk=getClk();

 }

int main(int argc, char *argv[])
{
    signal(SIGINT, SignalINThandler);
    signal(SIGUSR1,terminateprocess);
    initClk();
    char *Scheduling_Algo = argv[1];
    ProcessesNum=atoi(argv[2]);
     Quantum=atoi(argv[3]);
  
    processtable=(PCB *)malloc(sizeof(PCB) * ProcessesNum);
    //initilize ids with -1
    for(int i=0;i<ProcessesNum;i++)
    {
        processtable[i].id=-1;
         processtable[i].cummultiverunningtime=0;
    }
    
      
      
      
     
     int lasttaken=-1; //last element taken from shared memory 
     
      
       if (strcmp(Scheduling_Algo, "RR") == 0)
    {
        q_init(&CircularQueueRR);
      printf("Round Robin Algorithm starting...\n");
      processData *p=GetProcess();
      while(1)
       { 
    
        if(p->id!=-1&&p->id!=lasttaken)
       {
          printf("\n 2r %d \n",p->id);

       q_push(&CircularQueueRR,*p);
       lasttaken=p->id;
        printf("\n 1r\n");
       }  
      // printf("\n 13\n");
       
        if (getClk()>prevclk)
        {
          
           
          //  RR();
        }
 //if(ProcessesNum==FinishedProcessesNO) 
        // break; 
        
    }
    
         
           
    }


    else if (strcmp(Scheduling_Algo, "SRTN") == 0)
    {
        printf("Shortest Remaining Time Next Algorithm starting...\n");
        while (1)
        {
            SRTN();
        }
    }
    else
    {
        printf("Highest Prioity First Next Algorithm starting...\n");
       // PriorityQueueHPF = getPrioityQueueHPF();
        while (1)
        {
            HPF();
        }
    }

    // TODO implement the scheduler :)
    // upon termination release the clock resources.
    // destroyClk(true);
}
void SignalINThandler(int signnum)
{
    free(PriorityQueueHPF);
    exit(0);
}



void terminateprocess(int signnum)
{
    //print output file
    FinishedProcessesNO++;
   
    strcpy(processtable[RunningProcess->id-1].state, "finished");
     free(RunningProcess);
    RunningProcess=NULL;
    printf("\nprocess %d has finished \n" ,RunningProcess->id);
    

}

void StartProcess(processData*process)
{
    //fill the pcb for each process at the start of it
processtable[process->id-1].id=process->id;
processtable[process->id-1].waitingtime=getClk()-process->arrivaltime;
processtable[process->id-1].arrivaltime=process->arrivaltime;
processtable[process->id-1].runningtime=process->runningtime;
processtable[process->id-1].priority=process->priority;
processtable[process->id-1].cummultiverunningtime=0;
processtable[process->id-1].starttime=getClk();
processtable[process->id-1].responsetime=getClk()-process->arrivaltime;

strcpy(processtable[process->id-1].state, "running");
printf("\nprocess %d has started \n" ,RunningProcess->id);
int pid =fork();
{

    if(pid==0)
    {
         execl("./bin/process.out", "process.out", process->runningtime, NULL);
        
    }
    else{
         processtable[process->id-1].Pid=pid;

    }
    
}
//print in output file
}


void Calc()
{
        for(int i=0;i<ProcessesNum;i++)
        {
                    if(processtable[i].id!=-1)
                    {
                        if(processtable[i].state=="running") //if process is running increment the cummulitive running time
                    {
                        processtable[i].cummultiverunningtime++;
                    }


                    

                    if(processtable[i].state=="stopped") //if process is stopped increment the waiting time
                    {
                        processtable[i].waitingtime++;
                    }

                 }
        }
}

void ResumeProcess(processData * process)
{
    
  strcpy(processtable[process->id-1].state, "running");
  printf("\nprocess %d has resumed \n" ,RunningProcess->id);
  int pid=processtable[RunningProcess->id-1].Pid;
kill(pid,SIGCONT);

//print in output file
}
/////////////////////////////////////////////////////////////////////////
void PauseProcess(processData* process)
{

int pid=processtable[RunningProcess->id-1].Pid;
kill(pid,SIGSTOP);
strcpy(processtable[process->id-1].state, "stopped");
printf("\nprocess %d has paused \n" ,RunningProcess->id);
//print in output file
}