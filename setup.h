#ifndef SETUP
#define SETUP

//#define MAX_VAR_LEN 200

#define JOB_ARRIVAL 0
#define JOB_START 1
#define JOB_COMPLETION 2
#define END_SIMULATION 3

#define CPU 0
#define DISK1 1
#define DISK2 2
#define CPUQ 3
#define DISK1Q 4
#define DISK2Q 5
#define FINSTATE 6
//int pid is processing_ID

int CPU_MAX;
int CPU_MIN;
int DISK1_MAX;
int DISK1_MIN;
int DISK2_MIN;
int DISK2_MAX;

int INITIAL_TIME;
int FINAL_TIME = 10;
int ARRIVE_MAX;
int ARRIVE_MIN;
double QUIT_PROBABILITY;

int currentTime;
int jobs;
/*int numCPUJobsFinished;
int numDisk1JobsFinished;
int numDisk2JobsFinished;*/

//An event to be added to queue structure
typedef struct{
    int time; //Lowest time will have top priority in the priority queue, or event queue
    //int location; //cpu, disk1, disk2
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
//void enqueue(Queue *, int processID, int time, int status);
void pushQueue(Queue *, Event *);
Event *popQueue(Queue *);
Event *peek(Queue *);

#endif // SETUP
