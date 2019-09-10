#include <stdlib.h>
#include <stdio.h>
#include "setup.h"


void printQueue(Queue *q);

int main(){
    puts("String 1/5");
    Event *event1 = initializeEvent(10,0,0);
    puts("String 2/5");
    Event *event2 = initializeEvent(20,1,0);

    puts("String 3/5");
    Queue *eventQueue = initializeQueue();
    //eventQueue->size = 5;
    pushQueue(eventQueue, event1);
    pushQueue(eventQueue, event2);
    puts("String 4/5");
    //printQueue(eventQueue);
    puts("String 5/5");
    return 0;
}



//create head and tail variables
//make a head for each of the queues: priority_queue, cpu_queue, disk1_queue, disk2_queue

//input file: list of numbers fscanf for double

//Bubble Sort or some other sorting method for the
//priority queue when adding an event/process
Event *initializeEvent(int eventTime, int eventProcessID, int eventStatus){
    Event *newEvent = (Event *)malloc(sizeof(Event));
    newEvent->time = eventTime;
    newEvent->processID = eventProcessID;
    newEvent->status = eventStatus;
}

Queue *initializeQueue(){
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    /*if(queue==NULL){
        return NULL;
    }*/
    queue->size = 0;
    queue->head = NULL;
    //queue->tail = NULL;
    return queue;
}

//void pushPriorityQueue(Queue *queue, int eTime, int ePID, int eStatus)
void pushPriorityQueue(Queue *queue, Event *newEvent){
    // current event in the priority queue, if there is one
    Event *currentEvent = queue->head;
    //Event *eventAfter = currentEvent->nextPointer;
    if(queue->size == 0){
        queue->head = newEvent;
        queue->tail = newEvent;
        //queue->size++;
    }
    // multiple events in priority queue
    else{
        // the time of new event is not less than the time of head node, so there is lower priority
        if(newEvent->time <= currentEvent->time){
            //else new event has less than or equal time than time of head node, so high priority
            newEvent->nextPointer = queue->head; //assign new event to the head node
            queue->head = newEvent; //assign head node to the new event
        }
        else{
            //int i = 0;
            int i;
            for(i = 0; i < queue->size-1; i++){
                // if the new event's time is between the current event's time and some 'next event' node's time
                // make the 'next event' a node to new event, and then make new event the next node to current event
                if(newEvent->time >= currentEvent->time && newEvent->time <= currentEvent->nextPointer->time){
                    newEvent->nextPointer = currentEvent->nextPointer;
                    currentEvent->nextPointer = newEvent;
                    break; // end the loop once the position is found
                }
                // if no position is found during the iteration then assign next node to current event
                currentEvent = currentEvent->nextPointer;
                //i++;
            }
        }
        //queue->size++;
    }
    queue->size++;
}

//void enqueue(Queue *q, int time, int processID, int status)
void pushQueue(Queue *queue, Event *event){
    if(queue->size == 0){
        queue->head = event;
        queue->tail = event;
        queue->size++;
        return;
    }
    queue->tail->nextPointer = event;
    queue->tail = event;
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
        printf("PID: %d TIME: %d\n", queue->head->processID, queue->head->time);
        popQueue(queue);
    }
}
