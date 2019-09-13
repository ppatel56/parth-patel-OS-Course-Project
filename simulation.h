#ifndef SIMULATION
#define SIMULATION
#include <stdio.h>
#include "setup.h"
#include "log.h"

/*
Implement the event handling of the whole simulation. There is going to be a function for event handling, entering CPU, entering Disk 1 & 2 
*/

/* 
I need a random generator for the event's time, basically I need to pass in the MIN and MAX time intervals for each device then compute them
in the random generator equation to get the random time. Then subtract the random time from current time in the entering CPU, DISK 1, DISK 2,
or upon ARRIVAL used in eventHandler(). 
*/
int randomTime(int, int);

/*                                                      which DISK is it going to?
Queue *removedEvent, Queue *eventQueue, int currentTime, [int diskNum])  
removedEvent is an event removed from FIFO queue to be assigned to event thats going to enter device (CPU, DISK 1, or DISK 2)
*/  
void cpuArrival(Queue *, Queue *, int);
void diskArrival(Queue *, Queue *, int, int);

/*
Passing in all four queues, events that are to be processed, current time, and number of events

*/
void eventHandling(Queue *, Queue *, Queue *, Queue *, Event *,int, int);

// getting the number of events in the event queue by passing in event queue and current time
int getNumOfEvents(Queue *, int );
#endif // SIMULATION