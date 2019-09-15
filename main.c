#include <stdio.h>
#include <stdlib.h>
#include "setup.h"
#include "log.h"



#define CPU_IDLE 1
#define CPU_BUSY 2

#define DISK1_IDLE 1
#define DISK1_BUSY 2

#define DISK2_IDLE 1
#define DISK2_BUSY 2

#define IDLE 1
#define BUSY 2

int randomTime(int min, int max);

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

void eventHandling(Queue *eventQueue, Queue *cpuQueue, Queue *disk1Queue, Queue *disk2Queue, Event *currentEvent,Event *toBeProcessed[],int currentTime, int numOfEvents);

//void createNewEvent();
Event *nextEventArrival(Event *currentEvent);

Queue *eventQueue;
Queue *cpuQueue;
Queue *disk1Queue;
Queue *disk2Queue;

int nextProcessTime;

//nextProcessTime = currentTime + randTime(ARRIVE_MIN, ARRIVE_MAX);




int cpuState;
int disk1State;
int disk2State;
Event *nextEvent;

int main(){
    logFile = fopen("log.txt", "w");
    Event *currentEvent = initializeEvent();
    
    
    return 0;
}




/*
Implement the event handling of the whole simulation. There is going to be a function for event handling, entering CPU, entering Disk 1 & 2 
*/

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
/*Event createNewEvent(Queue *eventQueue){
    Event *newEvent = initializeEvent();
    newEvent->time = currentTime + randomTime(ARRIVE_MIN, ARRIVE_MIN);
    pushPriorityQueue(eventQueue, newEvent);
    return newEvent;
}
*/
Event *nextEventArrival(Event *currentEvent){
    Event *newEvent = initializeEvent();
    newEvent->time = currentTime + randomTime(ARRIVE_MIN, ARRIVE_MIN);
    pushPriorityQueue(eventQueue, newEvent);
    
   // Event *nextEvent = createNewEvent(eventQueue);
    currentEvent->status = CPU_ARRIVAL;
    pushQueue(cpuQueue, currentEvent);
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
void eventHandling(Queue *eventQueue, Queue *cpuQueue, Queue *disk1Queue, Queue *disk2Queue, Event *currentEvent,Event *toBeProcessed[],int currentTime, int numOfEvents){
    switch (currentTime->status){
        case EVENT_ARRIVAL:
           nextEvent = nextEventArrival(currentEvent);
           break;
        case CPU_ARRIVAL:
            enterCPU(eventQueue, cpuQueue, currentEvent, currentTime);
            break;
        case CPU_FIN:
            cpuFinished(cpuQueue, currentEvent, nextEvent,disk1Queue,disk2Queue,eventQueue,currentTime);
            break;
        case DISK1_ARRIVAL:
            enterDisk1(eventQueue,disk1Queue,currentEvent,currentTime);
            break;
        case DISK1_FIN:
            disk1Finished(disk1Queue,eventQueue,currentEvent,currentTime);
            break;
        case DISK2_ARRIVAL:
            enterDisk2(eventQueue,disk2Queue,currentEvent,currentTime);
            break;
        case DISK2_FIN:
            disk2Finished(disk2Queue,eventQueue, currentEvent, currentTime);
            break;
        case END_SIMULATION:
            fprintf(logFile,"\nThe simulation has reached Time: %d, program has ended\n",currentTime);
            break;
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