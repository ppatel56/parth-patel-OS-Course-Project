#include <stdio.h>
#include <stdlib.h>
#include "maintest.h"

#define EVENT_ARRIVAL 0
#define CPU_ARRIVAL 1
#define CPU_FIN 2
#define DISK1_ARRIVAL 3
#define DISK2_ARRIVAL 4
#define DISK1_FIN 5
#define DISK2_FIN 6
#define END_SIMULATION 7

#define CPU_IDLE 1
#define CPU_BUSY 2

#define DISK1_IDLE 1
#define DISK1_BUSY 2

#define DISK2_IDLE 1
#define DISK2_BUSY 2

#define IDLE 1
#define BUSY 2

int randomTime(int min, int max);

//create event function
Event *initializeEvent();
//priority queue
//void pushPriorityQueue(Queue *, int time, int pid, int status);
void pushPriorityQueue(Queue *eventQueue, Event *event);

Queue *initializeQueue();
void pushQueue(Queue *queue, Event *event);
Event *popQueue(Queue *queue);
Event *peek(Queue *queue);
void printQueue(Queue *queue);

int getNumOfEvents(Queue *eventQueue, int currentTime);

//void cpuArrival(Queue *eventQueue, Queue *cpuQueue, int currentTime, Event *eventArrived);

void enterCPU(Queue *eventQueue, Queue *cpuQueue, Event *currentEvent,int currentTime);

void cpuFinished(Queue *cpuQueue, Event *eventFinished, Event *nextEvent,Queue *disk1Queue, Queue *disk2Queue,Queue *eventQueue, int currentTime);

//void diskArrival(Queue *eventQueue, Queue *diskQueue, int currenntTime, int diskNum, Event *eventArrived);

void enterDisk1(Queue *eventQueue, Queue *disk1Queue, Event *currentEvent, int currentTime);

//void enterDisk2(Queue *eventQueue, Queue *disk2Queue, int currenntTime, int diskNum, Event *eventArrived);

void enterDisk2(Queue *eventQueue, Queue *disk2Queue, Event *currentEvent, int currentTime);

void disk1Finished(Queue *disk1Queue, Queue *eventQueue, Event *eventFinished, int currentTime);

void disk2Finished(Queue *disk2Queue, Queue *eventQueue, Event *eventFinished, int currentTime);

void eventHandling(Queue *eventQueue, Queue *cpuQueue, Queue *disk1Queue, Queue *disk2Queue, Event *currentEvent,int currentTime);

//void createNewEvent();
//Event *nextEventArrival(Event *currentEvent, Queue *eventQueue, Queue *cpuQueue);
Event *nextEventArrival(Event *currentEvent);

int getInputValuesInt();
double getInputValuesDouble();

int SEED;
int INITIAL_TIME;
int FINAL_TIME;
int ARRIVE_MAX;
int ARRIVE_MIN;
double QUIT_PROB;
int CPU_MIN;
int CPU_MAX;
int DISK1_MIN;
int DISK1_MAX;
int DISK2_MIN;
int DISK2_MAX;

Queue *eventQueue;
Queue *cpuQueue;
Queue *disk1Queue;
Queue *disk2Queue;



int nextProcessTime;
int eventProcessID = 0;
//nextProcessTime = currentTime + randTime(ARRIVE_MIN, ARRIVE_MAX);
int currentTime;



FILE *logFile;
FILE *inputFile;
FILE *statFile;

int cpuState;
int disk1State;
int disk2State;
Event *nextEvent;

/*
//An event to be added to queue structure
typedef struct Event{
    int time; //Lowest time will have top priority in the priority queue, or event queue

    int processID;
    int status;
    // int status can be the state of the process such as
    // process in the CPU, DISK1, DISK2, (CPU,DISK 1&2) Queues
    // or even Finished simulation each have numbers assigned

    struct Event *nextPointer;//pointer
}Event;

typedef struct{

    int size; //capacity of Queue
    Event *head; //front node
    Event *tail; //rear node

}Queue;*/




/* 
I need a random generator for the event's time, basically I need to pass in the MIN and MAX time intervals for each device then compute them
in the random generator equation to get the random time. Then subtract the random time from current time in the entering CPU, DISK 1, DISK 2,
or upon ARRIVAL used in eventHandler(). 
*/
int randomTime(int min, int max){
    int randomTimeNum = rand() % (max - min + 1) + min;
    return randomTimeNum;
}


/*
*Creating new events that happens at random time between the ARRIVE_MIN and ARRIVE_MAX, so at that time the event is created and pushed 
to event queue

*This will be in the main while loop so it will happen constantly 
*
*/
Event *createNewEvent(){
    Event *newEvent = initializeEvent();
    newEvent->time = currentTime + randomTime(ARRIVE_MIN, ARRIVE_MAX
					      );
    pushPriorityQueue(eventQueue, newEvent);
    return newEvent;
}

Event *nextEventArrival(Event *currentEvent){
    Event *newEvent = initializeEvent();
    puts("nextEventArrival:  newEvent initialized\n");
    newEvent->time = currentTime + randomTime(ARRIVE_MIN, ARRIVE_MAX);
    puts("nextEventArrival:  newEvent time is add to current time\n");
    puts("nextEventArrival:  newEvent about to enter the event queue\n");
    pushPriorityQueue(eventQueue, newEvent);
    puts("nextEventArrival:  newEvent added to event queue\n");
   // Event *nextEvent = createNewEvent(eventQueue);
    currentEvent->status = CPU_ARRIVAL;
    puts("nextEventArrival:  currentEvent status is CPU_ARRIVAL\n");
    pushQueue(cpuQueue, currentEvent);
    puts("nextEventArrival:  currentEvent is add to cpu queue\n");
     //put something about stats for cpu queue max size
    return newEvent;
}


/*                                                      which DISK is it going to?
Queue *removedEvent, Queue *eventQueue, int currentTime, [int diskNum])  
removedEvent is an event removed from FIFO queue to be assigned to event thats going to enter device (CPU, DISK 1, or DISK 2)

*I am passing in the cpuQueue because I'm going to pop the queue of the first event then check to see if CPU is busy, 
if it's not busy then the event is going to the CPU. 
*
*Note that another event will go into the CPU right away after the previous event leaves the CPU 
*/  
void enterCPU(Queue *eventQueue, Queue *cpuQueue, Event *currentEvent,int currentTime){
    //First check if cpu is busy using if(!CPU_BUSY && !isEmpty(cpuQueue))
    //Event *currentEvent = popQueue(eventQueue); //make sure to check if the queue isn't empty, maybe createNewEvent()
    //currentEvent->status = CPU_ARRIVAL;
    //pushQueue(cpuQueue, currentEvent);
    //Event *nextEvent = nextEventArrival(currentEvent);
    //eventAddedCPUQueue(eventQueue, cpuQueue);
    
    if(cpuState==CPU_IDLE){
        //Event *eventRemoved = popQueue(cpuQueue);
        currentEvent = popQueue(cpuQueue);
        fprintf(logFile,"\nEvent: %d has arrived at the CPU at Time: %d and CPU Queue size is %d\n",currentEvent->processID, currentTime, cpuQueue->size);
        //stat update cpu queue size (cpuQueue->size, currentTime)
        cpuState = CPU_BUSY;
    
        int timeAdded = currentTime + randomTime(CPU_MIN, CPU_MAX);
        
        int arrivalTime = currentEvent->time;
        currentEvent->time = timeAdded;

        fprintf(logFile, "\nEvent: %d  Time: %d is being processed by CPU and CPU Queue size is %d\n", currentEvent->processID, currentEvent->time, cpuQueue->size);
        currentEvent->status = CPU_FIN;
        //stat update cpu idle (cpu state, current time)
        //stat update cpu response time (arrival time, eventRemoved->time)
    }
    /*
    *First check if the cpu is busy, if it isn't then pop the cpu queue and put it in CPU and update the event's status 
    *Then record the time it enters in the CPU Queue, 
    *
    * Note** put something about stats for cpu queue max size 
    * Seperate cpu queue arrival and cpu arrival 
    */

}

/*
*diskNum is for which disk the event will choose to go thus will go to the disk queue  
*/


/*
*Check if the cpu queue is empty && cpu is open, it that is the case then print the cpu queue size for stat file
*Else if cpu queue is not empty && cpu is open then cpuArrival

*Remove the event from cpu and do the random generator  
*Create a random generator for where the event will go either exit the system or go into the disk
*If the event has exited the system, simply change the status of the event and just leave it and record it in stat file and log file
*If the event is not exiting the system then check to see if(disk1Queue->size < disk2Queue->size) or if else(disk2Queue->size < disk1Queue->size) 
then push them into the smallest disk queue
*Else if (disk1Queue->size == disk2Queue->size) then make another random generator to determine which queue the event will go into pick numbers 
like 0 or 1, 1 or 2, then whichever number is picked, push the event in the corresponding disk queue. 
*
*/
void cpuFinished(Queue *cpuQueue, Event *eventFinished, Event *nextEvent,Queue *disk1Queue, Queue *disk2Queue,Queue *eventQueue, int currentTime){
    fprintf(logFile, "\nEvent %d has left the CPU at Time %d\n",eventFinished->processID, currentTime);
    cpuState = CPU_IDLE;
    int minNum = 1, maxNum = 10;
    int quit = randomTime(minNum, maxNum);
    if(quit <= (int)(QUIT_PROB*10)){
        fprintf(logFile, "\nEvent: %d  Time: %d  has exited the system.", eventFinished->processID, eventFinished->time);
    }
    else{
        if(disk1Queue->size < disk2Queue->size){
            eventFinished->status = DISK1_ARRIVAL;
            pushQueue(disk1Queue, eventFinished);
            //stat update on max size of disk 1 queue
        }
        else if(disk1Queue->size > disk2Queue->size){
            eventFinished->status = DISK2_ARRIVAL;
            pushQueue(disk2Queue, eventFinished);
            //stat update on max size of disk 2 queue
        }
        else if(disk1Queue->size == disk2Queue->size){
            int diskChoice = randomTime(minNum, maxNum);
            
            if(diskChoice < 6){
                eventFinished->status = DISK1_ARRIVAL;
                pushQueue(disk1Queue, eventFinished);
            }
            else{
                eventFinished->status = DISK2_ARRIVAL;
                pushQueue(disk2Queue, eventFinished);
            }
        }
    }
    //stat update cpu finished
    //stat update cpu open (cpuState, currentTime)
    enterCPU(eventQueue, cpuQueue, nextEvent, currentTime); //next event is from the 
    //stat update cpu queue size
}


void enterDisk1(Queue *eventQueue, Queue *disk1Queue, Event *currentEvent, int currentTime){
    if(disk1State == DISK1_IDLE){
        fprintf(logFile,"\nEvent: %d has arrived at the Disk 1 at Time: %d and Disk 1 Queue size is %d\n",currentEvent->processID, currentTime, disk1Queue->size);
        //Event *eventRemoved = popQueue(disk1Queue);
        currentEvent = popQueue(disk1Queue);
        //stat update disk 1 queue size (disk1Queue->size, currentTime)
        disk1State = DISK1_BUSY;
    
        int timeAdded = currentTime + randomTime(DISK1_MIN, DISK1_MAX);
        
        int arrivalTime = currentEvent->time;
        currentEvent->time = timeAdded;

        fprintf(logFile, "\nEvent: %d  Time: %d is being processed by DISK 1 and Disk 1 Queue size is %d\n", currentEvent->processID, currentEvent->time, disk1Queue->size);
        currentEvent->status = DISK1_FIN;
        //stat update disk 1 idle (disk 1 state, current time)
        //stat update disk 1 response time (arrival time, eventRemoved->time)
    }
}

void disk1Finished(Queue *disk1Queue, Queue *eventQueue, Event *eventFinished, int currentTime){
    fprintf(logFile, "\nEvent %d has left DISK 1 at Time %d\n",eventFinished->processID, currentTime);
    disk1State = DISK1_IDLE;
    eventFinished->status = CPU_ARRIVAL;
    pushPriorityQueue(eventQueue,eventFinished);
    //stat update disk 1 is idle 
    //stat disk process is finished
    if(disk1Queue->size != 0){
        Event *nextEvent = popQueue(disk1Queue);
        enterDisk1(eventQueue, disk1Queue, nextEvent, currentTime);
        //stat update size of disk 1 queue
    }
}



void enterDisk2(Queue *eventQueue, Queue *disk2Queue, Event *currentEvent, int currentTime){
    if(disk2State == DISK2_IDLE){
        //Event *eventRemoved = popQueue(disk2Queue);
        currentEvent = popQueue(disk2Queue);
        fprintf(logFile,"\nEvent: %d has arrived at the Disk 2 at Time: %d and Disk 2 Queue size is %d\n",currentEvent->processID, currentTime, disk2Queue->size);
        //stat update disk 2 queue size (disk2Queue->size, currentTime)
        disk2State = DISK2_BUSY;
    
        int timeAdded = currentTime + randomTime(DISK2_MIN, DISK2_MAX);
        
        int arrivalTime = currentEvent->time;
        currentEvent->time = timeAdded;

        fprintf(logFile, "\nEvent: %d  Time: %d is being processed by DISK 2 and Disk 2 Queue size is %d\n", currentEvent->processID, currentEvent->time, disk2Queue->size);
        currentEvent->status = DISK2_FIN;
        //stat update disk2 idle (disk 2 state, current time)
        //stat update disk 2 response time (arrival time, currentEvent->time)
    }
}

void disk2Finished(Queue *disk2Queue, Queue *eventQueue, Event *eventFinished, int currentTime){
    fprintf(logFile, "\nEvent %d has left DISK 2 at Time %d\n",eventFinished->processID, currentTime);
    disk1State = DISK2_IDLE;
    eventFinished->status = CPU_ARRIVAL;
    pushPriorityQueue(eventQueue,eventFinished);
    //stat update disk 2 is idle 
    //stat disk process is finished
    if(disk2Queue->size != 0){
        Event *nextEvent = popQueue(disk2Queue);
        enterDisk2(eventQueue, disk2Queue, nextEvent, currentTime);
        //stat update size of disk 2 queue
    }
}



/*
Passing in all four queues, events that are to be processed, current time, and number of events

*/
void eventHandling(Queue *eventQueue, Queue *cpuQueue, Queue *disk1Queue, Queue *disk2Queue, Event *currentEvent,int currentTime){
    //puts("String 15/?\n");
    switch (currentEvent->status){
        //puts("String 15/?\n");
        case EVENT_ARRIVAL:
            puts("eventHandling:  Case of EVENT_ARRIVAL\n");
            //nextEvent = nextEventArrival(currentEvent,eventQueue,cpuQueue);
            puts("eventHandling:  About to create next event arrival\n");
            nextEvent = nextEventArrival(currentEvent);
            puts("eventHandling:  Created next event arrival\n");
            break;
        
        case CPU_ARRIVAL:
            puts("eventHandling:  Case of CPU_ARRIVAL\n");
            puts("eventHandling:  About to enter the CPU\n");
            enterCPU(eventQueue, cpuQueue, currentEvent, currentTime);
            puts("eventHandling:  Entered the CPU\n");
            break;
            
        case CPU_FIN:
            puts("eventHandling:  Case of CPU_FIN\n");
            puts("eventHandling:  About to CPU finished\n");
            cpuFinished(cpuQueue, currentEvent, nextEvent,disk1Queue,disk2Queue,eventQueue,currentTime);
            puts("eventHandling:  CPU finished process event\n");
            
            break;

        case DISK1_ARRIVAL:
            puts("eventHandling:  Case of DISK1_ARRIVAL\n");
            puts("eventHandling:  About to enter Disk 1\n");
            enterDisk1(eventQueue,disk1Queue,currentEvent,currentTime);
            puts("eventHandling:  Entered Disk 1\n");
            break;
        
        case DISK1_FIN:
            puts("eventHandling:  Case of DISK1_FIN\n");
            puts("eventHandling:  About to Disk 1 finished\n");
            disk1Finished(disk1Queue,eventQueue,currentEvent,currentTime);
            puts("eventHandling:  Finished with Disk 1\n");
            break;
        
        case DISK2_ARRIVAL:
            puts("eventHandling:  Case of DISK2_ARRIVAL\n");
            puts("eventHandling:  About to enter Disk 2\n");
            enterDisk2(eventQueue,disk2Queue,currentEvent,currentTime);
            puts("eventHandling:  Entered Disk 2\n");
            break;
        puts("String 33/?\n");
        case DISK2_FIN:
            puts("eventHandling:  Case of DISK2_FIN\n");
            puts("eventHandling:  About to Disk 2 finished\n");                    
            disk2Finished(disk2Queue,eventQueue, currentEvent, currentTime);
            puts("eventHandling:  Finished with Disk 2\n");
            break;
        
        case END_SIMULATION:
            puts("eventHandling:  Case of END_SIMULATION\n");
            puts("eventHandling:  About to end simulation\n");
            fprintf(logFile,"\nThe simulation has reached Time: %d, program has ended\n",currentTime);
            puts("eventHandling:  Simulation has ended\n");
            break;
        //puts("String 39/?\n");
    }
}

// getting the number of events in the event queue by passing in event queue and current time
int getNumOfEvents(Queue *eventQueue, int currentTime){
    Event *current = eventQueue->head;
    int counter = 0;
    while(current->time == currentTime){
        current = current->nextPointer;
        counter++;
    }
    return counter;
}


/*
*So far I have gotten majority of cpuArrival done. I am thinking about using createNewEvent() if priority queue is empty or something.
*Once I finish with cpuArrival, the disk1Arrival and disk2Arrival should be a little easier. 
*Some important notes, use random generator at the end of cpuFinish() so that I can determine if event goes to disks or exit simulation.
    *If event is exiting the simulation, then just print to log that the event has exited the system at its current time/timeAdded
    *If event is going to disk, then obviously do if statements comparing the size of the disk queues to see which one is smaller.
        *If one of the disk queue is smaller than the other, push the event to that disk queue
        *Else if the size of the disk queues are equal, then do a random generator to choice which disk queue the event should enter.
            *This will probably lead to more if statements.  
        *Once I finshed with the diskArrival for each disk, work on the diskFinished for each disk 
            *Which at the end of those functions, the finished event will be pushed into the event queue which will go into the cpu queue soon after. 
*After that the event handler function is last function simulation function and then do testing.
*If testing is successful, then do stat files.



*/


/*
int main(){
    puts("String 1/5");
    Event *event1 = initializeEvent(10,0,0);
    puts("String 2/5");
    Event *event2 = initializeEvent(8,1,0);
    Event *event3 = initializeEvent(9,2,0);
    puts("String 3/5");
    Queue *eventQueue = initializeQueue();
    //eventQueue->size = 5;
    pushPriorityQueue(eventQueue, event1);
    pushPriorityQueue(eventQueue, event2);
    pushPriorityQueue(eventQueue, event3);
    puts("String 4/5");
    printQueue(eventQueue);
    //printQueue(eventQueue);
    puts("String 5/5");
    return 0;
}*/

//maybe do random time for arrival time

//create head and tail variables
//make a head for each of the queues: priority_queue, cpu_queue, disk1_queue, disk2_queue

//input file: list of numbers fscanf for double

//Bubble Sort or some other sorting method for the
//priority queue when adding an event/process
/*
*Make first initial event and final event->status = FIN_STATE
*After the initial and final events, do random time 
*/
Event *initializeEvent(){
    Event *newEvent = (Event *)malloc(sizeof(Event));
    //eventStatus = EVENT_ARRIVAL;
    //newEvent->time = eventTime;
    newEvent->processID = eventProcessID;
    newEvent->status = EVENT_ARRIVAL;
    ++eventProcessID;
    return newEvent;
}

Queue *initializeQueue(){
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    /*if(queue==NULL){
        return NULL;
    }*/
    queue->size = 0;
    queue->head = NULL;
    
    return queue;
}

void pushPriorityQueue(Queue *queue, Event *newEvent){
    // current event in the priority queue, if there is one
    Event *currentEvent = queue->head;
    
    //temporary pointer of the new event
    Event *tempPtr = (Event*)malloc(sizeof(Event));
    tempPtr->time = newEvent->time;
    tempPtr->processID = newEvent->processID;
    tempPtr->status = newEvent->status;
    
    if (queue->size == 0) {
        queue->head = queue-> tail = tempPtr;
    }
    // multiple events in priority queue 
    else {
        // the time of new event is less than or equal to the time of head node, so there is higher priority
        if (tempPtr->time <= currentEvent->time) {
            tempPtr->nextPointer = queue->head;
	        queue->head = tempPtr;
        }
        else {
            //If new event's time is greater than the head node, there is lower priority
            int i;
            //iterate through the priority queue in order to make sure if any other events have higher or lower priority than the newEvent
            for (i = 0; i < queue->size-1; i++) {
                //If the new event's position should be somewhere in the middle of the queue (not front or end of queue)
                if (tempPtr->time >= currentEvent->time && tempPtr->time <= currentEvent->nextPointer->time) {
                    tempPtr->nextPointer = currentEvent->nextPointer;
                    currentEvent->nextPointer = tempPtr;
                    break;
	            }
                currentEvent = currentEvent->nextPointer;
	        }
	        if (i == queue-> size - 1) {
	            currentEvent->nextPointer = tempPtr;
	            queue->tail = tempPtr;
	        }

        }
    }
    queue->size++;
}
    // puts("pushPriorityQueue:   if there is a currentEvent as queue head\n");
    //Event *eventAfter = currentEvent->nextPointer;
    /* if(queue->size == 0 || newEvent->time >= queue->tail->time){ */
    /*     puts("pushPriorityQueue:   if queue->size == 0\n"); */
    /*     queue->head = newEvent; */
    /*     puts("pushPriorityQueue:   if queue->size == 0 then queue->head = newEvent\n"); */
    /*     queue->tail = newEvent; */
    /*     puts("pushPriorityQueue:   if queue->size == 0 then queue->tail = newEvent\n"); */
    /*     queue->size += 1; */
    /*     //pushQueue(queue,newEvent); */
    /* } */
    /* // multiple events in priority queue */
    /* else{ */
    /*     // the time of new event is less than or equal to the time of head node, so there is higher priority */
    /*     if(newEvent->time <= currentEvent->time){ */
    /*         puts("pushPriorityQueue:  if newEvent time is less or equal to currentEvent time\n"); */
    /*         newEvent->nextPointer = queue->head; //assign new event to the head node */
    /*         puts("pushPriorityQueue:   assign newEvent to the head node\n"); */
    /*         queue->head = newEvent; //assign head node to the new event */
    /*         puts("pushPriorityQueue:   assign head node to the newEvent\n"); */
    /*     } */
    /*     else{ */
    /*         puts("pushPriorityQueue:  else when newEvent time is greater than currentEvent time\n"); */
    /*         //int i = 0; */
    /*         int i; */
    /*         //for(i = 0; i < queue->size-1; i++){ */
    /*         for(i=0;i<queue->size-1;i++){ */
    /*             puts("pushPriorityQueue:   iterate through the priority queue\n"); */
    /*             // if the new event's time is between the current event's time and some 'next event' node's time */
    /*             // make the 'next event' a node to new event, and then make new event the next node to current event */
    /*             //else new event has greateer time than time of head node, so lower priority */
    /*             if(newEvent->time > currentEvent->time && newEvent->time <= currentEvent->nextPointer->time){ */
    /*                 puts("pushPriorityQueue:   if there is an event time greater than newEvent time then try to switch it\n"); */
    /*                 newEvent->nextPointer = currentEvent->nextPointer; */
    /*                 puts("pushPriorityQueue:   newEvent is switching places\n"); */
    /*                 currentEvent->nextPointer = newEvent; */
    /*                 puts("pushPriorityQueue:   newEvent switched places\n"); */
    /*                 break; // end the loop once the position is found */
    /*             } */
    /*             // if no position is found during the iteration then assign next node to current event */
    /*             currentEvent = currentEvent->nextPointer; */
    /*             puts("pushPriorityQueue:   if no position is found during the iteration then assign next node to current event\n"); */
    /*             //i++; */
    /*         } */
    /*     } */
    
//        queue->size += 1;
//  }
    //queue->size++;
//  puts("pushPriorityQueue:   increment the eventQueue by 1\n");
//}


/*void pushQueue(Queue *queue, Event *event){
    if(queue->size == 0){
        queue->head = event;
        queue->tail = event;
        queue->size +=1;
        return;
    }
    queue->tail->nextPointer = event;
    queue->tail = event;
    queue->size += 1;
}*/

void pushQueue(Queue *queue, Event *event){
    Event *tempPointer = (Event*)malloc(sizeof(Event));
    tempPointer->time = event->time;
    tempPointer->processID = event->processID;
    tempPointer->status = event->status;
    if (queue->size == 0) {
        queue->head = queue-> tail = tempPointer;
    }
    queue->tail->nextPointer = tempPointer;
    queue->tail = tempPointer;
    queue->size++;
}

//Works for both FIFO queue and Priority queue
Event *popQueue(Queue *queue){
    if(queue->size == 0){
        return NULL;
    }
    //removed event from a FIFO queue
    Event *removedEvent = queue->head;
    queue->head = queue->head->nextPointer;
    queue->size--;
    if(queue->size == 0){
        queue->tail = NULL;
    }
    // return the removed Event
    return removedEvent;
}

//Works for both priority queue and FIFIO queue
Event *peek(Queue *queue){
    if(queue->head == NULL){
        return NULL;
    }
    //return queue->head->this;
    return queue->head;
}

void printQueue(Queue *queue){
    int i;
    for(i=0; i<queue->size; i++){
        printf("Event %d: PID: %d TIME: %d STATUS: %d\n",i ,queue->head->processID, queue->head->time, queue->head->status);
        //popQueue(queue);
        queue->head = queue->head->nextPointer; // the next event is assigned as head of queue and then print out 
        puts("\nprintQueue:  Done iteration\n");
    }
    puts("printQueue:  function end\n");
}

int getInputValuesInt(){
    int value;
    char strVariable[255];
    fscanf(inputFile,"%s",strVariable);
    fscanf(inputFile, "%d",&value);
    return value;
}

double getInputValuesDouble(){
    double value;
    char strVariable[255];
    fscanf(inputFile,"%s",strVariable);
    fscanf(inputFile, "%lf", &value);
    return value;
}
















int main(){
    
    inputFile = fopen("input.txt", "r");
    puts("Main:   Opened input file to read\n");
    SEED = getInputValuesInt();
    //puts("String 3/?\n");
    INITIAL_TIME = getInputValuesInt();
    //puts("String /?\n");
    FINAL_TIME = getInputValuesInt();
    ARRIVE_MIN = getInputValuesInt();
    ARRIVE_MAX = getInputValuesInt();
    QUIT_PROB = getInputValuesDouble();
    CPU_MIN = getInputValuesInt();
    CPU_MAX = getInputValuesInt();
    DISK1_MIN = getInputValuesInt();
    DISK1_MAX = getInputValuesInt();
    DISK2_MIN = getInputValuesInt();
    DISK2_MAX = getInputValuesInt();
    puts("Main:   Got all the values from the input file\n");
    fclose(inputFile);
    puts("Main:   Input file is closed\n");
    //stat initialize (FINAL_TIME)
    logFile = fopen("log.txt", "w");
    puts("Main:   Opened log file to write in the event logs\n");
    //initialize the queues
    eventQueue = initializeQueue();
    cpuQueue = initializeQueue();
    disk1Queue = initializeQueue();
    disk2Queue = initializeQueue();
    puts("Main:   Initialized all the queues\n");
    //creating the first and last events
    Event *firstEvent = initializeEvent();
    puts("Main:   Initialized the first event\n");
    firstEvent->time = INITIAL_TIME;
    Event *lastEvent = initializeEvent();
    puts("Main:   Initialized the last event\n");
    lastEvent->status = END_SIMULATION;
    lastEvent->time = FINAL_TIME;
    puts("Main:   About to add the first event to the event queue\n");
    pushPriorityQueue(eventQueue,firstEvent);
    puts("Main:   Added the first event to event queue\n");
    puts("Main:   About to add the last event to event queue\n");
    pushPriorityQueue(eventQueue,lastEvent);
    puts("Main:   Added the last event to event queue\n");
    //printQueue(eventQueue);
    
    //main while loop
    while(eventQueue->size && currentTime < FINAL_TIME){
        puts("Main:   Going through main while loop\n");
	Event *currentEvent = popQueue(eventQueue);
        puts("Main:   Removed the current event in the event queue\n");
        //currentTime = currentEvent->time;
        puts("Main:   Current time is the current event's time\n");
        puts("Main:   About to do event handling.\n");
        eventHandling(eventQueue,cpuQueue,disk1Queue,disk2Queue,currentEvent,currentTime);
        puts("Main:   Event handling successful\n");
        //getting number of events count to record
        //for(int i=0;i<){
	currentTime++;  
        //}
    }
    puts("Main:   Finished the main while loop\n");
    

    //Priority Queue testing 
    /*
    puts("String 1/5");
    Event *event1 = initializeEvent();
    puts("String 2/5");
    Event *event2 = initializeEvent();
    Event *event3 = initializeEvent();
    puts("String 3/5");
    Queue *eventQueue = initializeQueue();
    //eventQueue->size = 5;
    pushPriorityQueue(eventQueue, event1);
    pushPriorityQueue(eventQueue, event2);
    pushPriorityQueue(eventQueue, event3);
    puts("String 4/5");
    printQueue(eventQueue);
    //printQueue(eventQueue);
    puts("String 5/5");
    */

    puts("Main:   End of main loop\n");
    return 0;
}
