#ifndef MAINTEST
#define MAINTEST

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

}Queue;
#endif // MAINTEST