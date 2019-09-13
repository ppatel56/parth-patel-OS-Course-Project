#include <stdio.h>
#include <stdlib.h>
#include "setup.h"
#include "log.h"


#define CPU_BUSY 1
#define DISK1_BUSY 1
#define DISK2_BUSY 1

/*
#define EVENT_ARRIVAL 0
#define EVENT_START 1
#define EVENT_COMPLETION 2
#define END_SIMULATION 3

#define CPU 4
#define DISK1 5
#define DISK2 6
#define CPUQ 7
#define DISK1Q 8
#define DISK2Q 9
#define CPU_ARRIVAL 4
#define CPU_FIN 5
#define DISK_ARRIVAL 6
#define DISK1_FIN 7
#define DISK2_FIN 8
#define FIN_STATE 9

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

int currentTime;

int numCPUJobsFinished;
int numDisk1JobsFinished;
int numDisk2JobsFinished;
*/
int randomTime(int min, int max);
int getNumOfEvents(Queue *eventQueue, int currentTime);

//void cpuArrival(Queue *eventQueue, Queue *cpuQueue, int currentTime, Event *eventArrived);

void cpuArrival(Queue *eventQueue, Queue *cpuQueue, int currentTime, Event *eventArrived);

void cpuFinished(Queue *cpuQueue, Event *eventFinished);

//void diskArrival(Queue *eventQueue, Queue *diskQueue, int currenntTime, int diskNum, Event *eventArrived);

void diskArrival(Queue *eventQueue, Queue *diskQueue, int currenntTime, int diskNum, Event *eventArrived);

void disk1Finished(Queue *disk1Queue, Queue *eventQueue, Event *eventFinished);

void disk2Finished(Queue *disk2Queue, Queue *eventQueue, Event *eventFinished);

void createNewEvent();

int nextProcessTime;
nextProcessTime = currentTime + randTime(ARRIVE_MIN, ARRIVE_MAX);

logFile = open("log.txt", "w");

int main(){
    int result = randomTime(1,2);
    printf("Result = %d\n", result);
    
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
    int randomTimeNum = min + rand() % (max - min + 1);
    return randomTimeNum;
}


/*
*Creating new events that happens at random time between the ARRIVE_MIN and ARRIVE_MAX, so at that time the event is created and pushed 
to event queue

*This will be in the main while loop so it will happen constantly 
*
*/
void createNewEvent(Queue *eventQueue){
    Event *newEvent = initializeEvent();
    newEvent->time = currentTime + randomTime(ARRIVE_MIN, ARRIVE_MIN);
    pushPriorityQueue(eventQueue, newEvent);
}



/*                                                      which DISK is it going to?
Queue *removedEvent, Queue *eventQueue, int currentTime, [int diskNum])  
removedEvent is an event removed from FIFO queue to be assigned to event thats going to enter device (CPU, DISK 1, or DISK 2)

*I am passing in the cpuQueue because I'm going to pop the queue of the first event then check to see if CPU is busy, 
if it's not busy then the event is going to the CPU. 
*
*Note that another event will go into the CPU right away after the previous event leaves the CPU 
*/  
void cpuArrival(Queue *eventQueue, Queue *cpuQueue, int currentTime){
    //First check if cpu is busy using if(!CPU_BUSY && !isEmpty(cpuQueue))
    Event *currentEvent = popQueue(eventQueue); //make sure to check if the queue isn't empty, maybe createNewEvent()
    pushQueue(currentEvent);

    if(!CPU_BUSY){
        int timeAdd = currentTime + randomTime(CPU_MIN, CPU_MAX);
        Event *eventRemoved = popQueue(cpuQueue);
        eventRemoved->status = CPU;
        fprintf(logFile, "Event %d has entered CPU at time %d", eventRemoved->processID, eventRemoved->time);
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


void cpuFinished(Queue *cpuQueue, Event *eventFinished, Queue *disk1Queue, Queue *disk2Queue){
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
}


void diskArrival(Queue *eventQueue, Queue *diskQueue, int currenntTime, int diskNum){
    
}

/*
Passing in all four queues, events that are to be processed, current time, and number of events

*/
void eventHandling(Queue *eventQueue, Queue *cpuQueue, Queue *disk1Queue, Queue *disk2Queue, Event *toBeProcessed[],int currentTime, int numOfEvents);

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