#include <stdio.h>
#include <stdlib.h>
#include "setup.h"
#include "log.h"


#define CPU_BUSY 1
#define DISK1_BUSY 1
#define DISK2_BUSY 1

int randomTime(int min, int max);
int getNumOfEvents(Queue *eventQueue, int currentTime);
/*
*I am passing in the cpuQueue because I'm going to pop the queue of the first event then check to see if CPU is busy, if it's not busy then the event is
going to the CPU. 

*/
void cpuArrival(Queue *eventQueue, Queue *cpuQueue, int currentTime);
/*
*I will pass in cpu queue and the event finished in cpu so that I can put it back into the cpu queue or exit the system, record the time it finishes. 
*Also I'm passing in the cpuQueue so that I can bring in another event once CPU is not busy (if it's time is ready with random time? *Note need to confirm with TA)
*
*/
void cpuFinished(Queue *cpuQueue, Event *eventFinished);

void diskArrival(Queue *eventQueue, Queue *diskQueue, int currenntTime, int diskNum);


void disk1Finished(Queue *disk1Queue, Queue *);




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

/*                                                      which DISK is it going to?
Queue *removedEvent, Queue *eventQueue, int currentTime, [int diskNum])  
removedEvent is an event removed from FIFO queue to be assigned to event thats going to enter device (CPU, DISK 1, or DISK 2)
*/  
void cpuArrival(Queue *, Queue *, int){

}
void diskArrival(Queue *, Queue *, int, int){

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