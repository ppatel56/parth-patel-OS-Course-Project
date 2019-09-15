#ifndef SETUP
#define SETUP

#define EVENT_ARRIVAL 0
#define CPU_ARRIVAL 1
#define CPU_FIN 2
#define DISK1_ARRIVAL 3
#define DISK2_ARRIVAL 4
#define DISK1_FIN 5
#define DISK2_FIN 6
#define END_SIMULATION 7



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
int eventProcessID = 0;
/*int numCPUJobsFinished;
int numDisk1JobsFinished;
int numDisk2JobsFinished;*/

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

//create event function
Event *initializeEvent();
//priority queue
//void pushPriorityQueue(Queue *, int time, int pid, int status);
void pushPriorityQueue(Queue *, Event *);

Queue *initializeQueue();
void pushQueue(Queue *, Event *);
Event *popQueue(Queue *);
Event *peek(Queue *);
void printQueue(Queue *q);
#endif // SETUP
