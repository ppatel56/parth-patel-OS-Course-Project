#ifndef SETUP
#define SETUP

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
Event *initializeEvent(int, int, int);
//priority queue
//void pushPriorityQueue(Queue *, int time, int pid, int status);
void pushPriorityQueue(Queue *, Event *);

Queue *initializeQueue();
int isEmpty(Queue *);
void pushQueue(Queue *, Event *);
Event *popQueue(Queue *);
Event *peek(Queue *);
void printQueue(Queue *q);
#endif // SETUP
