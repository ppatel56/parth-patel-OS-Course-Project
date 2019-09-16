#include <stdio.h>
#include <stdlib.h>
#include "maintest.h"


#define EVENT_ARRIVAL 1
#define CPU_ARRIVAL 2
#define CPU_FIN 3
#define DISK_ARRIVAL 4
#define DISK1_FIN 5
#define DISK2_FIN 6
#define END_SIMULATION 7


#define IDLE 1
#define BUSY 0

int randomTime(int min, int max);

void enterCPU(Event *currentEvent);

void cpuFinished(Event *eventFinished);

void enterDisk(Event* currentEvent);

void disk1Finished(Event *eventFinished);

void disk2Finished(Event *eventFinished);

void eventHandling(Event *currentEvent);

void nextEventArrival(Event *currentEvent);


int getInputValuesInt();


int initialTime;
int finalTime;
int arriveMin;
int arriveMax;
int quitProb;
int cpuMin;
int cpuMax;
int disk1Min;
int disk1Max;
int disk2Min;
int disk2Max;


Queue *eventQueue;
Queue *cpuQueue;
Queue *disk1Queue;
Queue *disk2Queue;

int eventProcessID = 0;

int currentTime;

FILE *logFile;
FILE *inputFile;
FILE *statFile;

int cpuState;
int disk1State;
int disk2State;

/* 
I need a random generator for the event's time, basically I need to pass in the MIN and MAX time intervals for each device then compute them
in the random generator equation to get the random time. Then subtract the random time from current time in the entering CPU, DISK 1, DISK 2,
or upon ARRIVAL used in eventHandler(). 
*/
int randomTime(int min, int max){
    int randomTimeNum = rand() % (max - min + 1) + min;
    return randomTimeNum;
}

Event *initializeEvent(){
    Event *newEvent = (Event *)malloc(sizeof(Event));
    //eventStatus = EVENT_ARRIVAL;
    //newEvent->time = eventTime;
    newEvent->processID = eventProcessID;
    newEvent->status = EVENT_ARRIVAL;
    eventProcessID++;
    return newEvent;
}

//Creating a new node using allocated memory and assigning an event into that node then returning that new node
//Useful in dealing with manipulating in the queue data structure
Node *newNode(Event *event){
    Node *node = (Node *)malloc(sizeof(Node));
    node->event = event;
    return node;
}
//Initializing a queue whether it be FIFO Queue or priority queue, the size is equal to 0
Queue *initializeQueue(){
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->size = 0;
    return queue;
}

//Event is put inside the newly created node, then function checks if queue is empty, if so then new node is first node
//Otherwise, new node would be placed after an exisiting node in the queue
void pushQueue(Queue *queue, Event *event){
    Node *addingNode = newNode(event);
    if(queue->size == 0){
        queue->firstNode = addingNode;
    }
    else{
        queue->lastNode->nextNode = addingNode;
    }
    queue->lastNode = addingNode;
    queue->size++;
}

void pushPriorityQueue(Queue *eventQueue, Event *newEvent){
    //If the event queue is empty or if the newEvent's time is greater than or equal to the previous event's time
    //Add the newEvent in normally like a FIFO queue, hence why I am using reusing code for FIFO pushQueue
    if(eventQueue->size == 0 || newEvent->time >= eventQueue->lastNode->event->time){
        Node *addingNode = newNode(newEvent);
        if(eventQueue->size == 0){
            eventQueue->firstNode = addingNode;
        }
        else{
            eventQueue->lastNode->nextNode = addingNode;
        }
        eventQueue->lastNode = addingNode;
        eventQueue->size++;
    }
    //Else the newEvent's time priority places it at first or middle of the event queue
    //I will use the while loop to find the newEvent's position either at the beginning of event queue or somewhere else (not last)
    else{
        Node *previousNode;
        Node *currentNode = eventQueue->firstNode;
        Node *addingNode = newNode(newEvent);
        while(newEvent->time >= currentNode->event->time){
            previousNode = currentNode;
            currentNode = currentNode->nextNode;
        }
        
        //
        if(currentNode != eventQueue->firstNode){
            previousNode->nextNode = addingNode;
        }
        else{
            eventQueue->firstNode = addingNode;
        }
        addingNode->nextNode = currentNode;
        eventQueue->size++;
    }
}

//Removing the first event in the queue
//event is equal to NULL because we are actually removing the node that contains the event
//Then event will equal to the removedNode if the queue was not empty otherwise output would be NULL

Event *popQueue(Queue *queue){
    Node *removedNode;
    Event *event = NULL;
    if(queue->size != 0){
        removedNode = queue->firstNode;
        event = removedNode->event;
        //Incase if there is are more than 1 elements in the queue, make sure to make the node after removedNode the first node
        if(queue->size > 1){
            queue->firstNode = removedNode->nextNode;
        }
        queue->size--;
    }
    return event;
}
//This function is used to get the integer values from input.txt 
//strVariable is for the variable strings with 200 characters just in case 
int getInputValuesInt(){
    int value;
    char strVariable[200];
    fscanf(inputFile,"%s",strVariable);
    fscanf(inputFile, "%d",&value);
    return value;
}

/*
*Creating new events that happens at random time between the arriveMin and arriveMax, so at that time the event is created and pushed 
to event queue

*This will be in the main while loop so it will happen constantly 
*
*/



void nextEventArrival(Event *newEvent){
    //Creating next event to enter the simulation.
    Event *nextEvent = initializeEvent();
    nextEvent->time = currentTime + randomTime(arriveMin,arriveMax);
    pushPriorityQueue(eventQueue, nextEvent);
    //Send the new event to CPU/or CPU queue if CPU is BUSY
    newEvent->status = CPU_ARRIVAL;
}

/*
*
*/
void enterCPU(Event* currentEvent){
    if(cpuState == IDLE){
        fprintf(logFile, "\nTime: %d  --  Event %d is being processed.", currentTime, currentEvent->processID);
        cpuState = BUSY;
        //statUpdateCpuIdle(cpuState, currentTime);
        int jobArrivalTime = currentEvent->time;
        currentEvent->time = currentTime + randomTime(cpuMin,cpuMax);
        currentEvent->status = CPU_FIN;
        //statUpdateCpuResponseTime(jobArrivalTime, currentEvent->time);
        pushPriorityQueue(eventQueue, currentEvent);
    } else{
        pushQueue(cpuQueue, currentEvent);
        //statUpdateCpuQSize(cpuQueue->size, currentTime);
    }
}

/*
*First make the CPU idle so that the next event can come in. 
*Remove the event from cpu and do the random generator  
*Create a random generator for where the event will go either exit the system or go into the disk
*If the event has exited the system, simply change the status of the event and just leave it and record it in stat file and log file
*/
void cpuFinished(Event* currentEvent){
    cpuState = IDLE;
    //statCpuJobFinished();
    //statUpdateCpuIdle(cpuState, currentTime);
    if(cpuQueue->size != 0){
        enterCPU(popQueue(cpuQueue));
        //statUpdateCpuQSize(cpuQueue->size, currentTime);
    }
    int min = 1, max = 100;
    if(randomTime(min,max) <= quitProb){
        fprintf(logFile, "\nTime: %d  --  Event %d is exiting the system", currentTime, currentEvent->processID);
        free(currentEvent);
    }else{
        currentEvent->status = DISK_ARRIVAL;
        pushPriorityQueue(eventQueue, currentEvent);
    }
}


//Pointer for diskQueue so that it would change on what disk it points depending on the disk1Queue->size or disk2Queue->size 
//Pointer for the diskState is the same reason as above statement and this would help lead into what function the event would be sent into
void enterDisk(Event* currentEvent){
    
    Queue *diskQueue = disk1Queue;
    int *diskState = &disk1State;
    int diskNum = 1;
    int eventChoice = DISK1_FIN;
    int arrivalTime;
    int min = 0, max = 1;
    if ((disk1Queue->size == disk2Queue->size && randomTime(min,max)) || (disk2Queue->size < disk1Queue->size)){
        diskQueue = disk2Queue;
        diskState = &disk2State;
        diskNum = 2;
        eventChoice = DISK2_FIN;
    }
    if(*diskState == IDLE){
        *diskState = BUSY;
        //statUpdateDiskIdle(diskNumber, *diskStatus, currentTime);
        arrivalTime = currentEvent->time;
        currentEvent->time = currentTime + randomTime(disk1Min,disk1Max);
        currentEvent->status = eventChoice;
        //statUpdateDiskResponseTime(diskNumber, jobArrivalTime, currentEvent->time);
        pushPriorityQueue(eventQueue, currentEvent);
    }else{
        pushQueue(diskQueue, currentEvent);
        //statUpdateDiskQSize(diskNumber, diskQueue->size, currentTime);
    }
}


void disk1Finished(Event *currentEvent){
    disk1State = IDLE;
    //statDiskJobFinished(1);
    //statUpdateDiskIdle(1, disk1State, currentTime);
    if(disk1Queue->size != 0){
        enterDisk(popQueue(disk1Queue));
        //statUpdateDiskQSize(1, disk1Queue->size, currentTime);
    }
    currentEvent->status = CPU_ARRIVAL;
    pushPriorityQueue(eventQueue, currentEvent);
}


void disk2Finished(Event *currentEvent){
    disk2State = IDLE;
    //statDiskJobFinished(2);
    //statUpdateDiskIdle(2, disk2State, currentTime);
    if(disk2Queue->size != 0){
        enterDisk(popQueue(disk2Queue));
        //statUpdateDiskQSize(2, disk2Queue->size, currentTime);
    }
    currentEvent->status = CPU_ARRIVAL;
    pushPriorityQueue(eventQueue, currentEvent);
}


/*
As the function name suggests, the eventHandling handles all the processes in the simulation and is depended on the currentEvent's status.
*Since the event handling ran by a switch case, the cases are the define constants which are depended on the currentEvent's status
*The event will be brought back into the priority queue with a new status so that it will go through the event handling to the corresponding
case  
*/
void eventHandling(Event* currentEvent){
    switch (currentEvent->status){
        /*When a new event enters the system, the next event is created and added to the eventQueue
         *Then this event is sent to the CPU*/
        case EVENT_ARRIVAL:
            nextEventArrival(currentEvent);
            //Break doesn't allow other events enter the event queue.
        case CPU_ARRIVAL:
            fprintf(logFile, "\nTime: %d  --  Event %d arrived at CPU -- CPU queue length: %d", currentTime, currentEvent->processID, cpuQueue->size);
            enterCPU(currentEvent);
            break;
        case CPU_FIN:
            fprintf(logFile, "\nTime: %d  --  Event %d finished at CPU", currentTime, currentEvent->processID);
            cpuFinished(currentEvent);
            break;
        case DISK_ARRIVAL:
            fprintf(logFile, "\nTime: %d  --  Event %d arrived at Disk", currentTime, currentEvent->processID);
            enterDisk(currentEvent);
            break;
        case DISK1_FIN :
            fprintf(logFile, "\nTime: %d  --  Event %d finished at Disk 1", currentTime, currentEvent->processID);
            disk1Finished(currentEvent);
            break;
        case DISK2_FIN :
            fprintf(logFile, "\nTime: %d  --  Event %d finished with Disk 2", currentTime, currentEvent->processID);
            disk2Finished(currentEvent);
            break;
        case END_SIMULATION :
            fprintf(logFile, "\nTime %d  --  Simulation has reached the end, exiting program\n", currentTime);
            break;
    }
}













int main(){
    //initSystemValues();
    currentTime = initialTime;
    cpuState = !BUSY;
    disk1State = !BUSY;
    disk2State = !BUSY;
    
    cpuQueue = initializeQueue();
    disk1Queue = initializeQueue();
    disk2Queue = initializeQueue();
    eventQueue = initializeQueue();

    inputFile = fopen("input.txt", "r");
    initialTime = getInputValuesInt();
    finalTime = getInputValuesInt();
    arriveMin = getInputValuesInt();
    arriveMax = getInputValuesInt();
    quitProb = getInputValuesInt();
    cpuMin = getInputValuesInt();
    cpuMax = getInputValuesInt();
    disk1Min = getInputValuesInt();
    disk1Max = getInputValuesInt();
    disk2Min = getInputValuesInt();
    disk2Max = getInputValuesInt();
    fclose(inputFile);
    
    logFile = fopen("logfile.txt", "w");

    Event* lastJob = initializeEvent();
    lastJob->time = finalTime;
    lastJob->status = END_SIMULATION;
    pushPriorityQueue(eventQueue, lastJob);
    
    /*Create first event and add it to the eventqueue at time 0*/
    Event* job1 = initializeEvent();
    job1->time = 0;
    pushPriorityQueue(eventQueue, job1);
    while(eventQueue->size != 0 && currentTime < finalTime){
        Event* nextEvent = popQueue(eventQueue);
        currentTime = nextEvent->time;
        eventHandling(nextEvent);
    }
    return 0;
}


