#ifndef MAINTEST
#define MAINTEST

typedef struct Event{
  int time;
  int processID;
  int status;
}Event;

Event *initializeEvent();


typedef struct node{
    struct node* nextNode;
    Event *event;
}Node;

typedef struct queue{
    Node *firstNode;
    Node *lastNode;
    int size;
}Queue;

Node *newNode(Event* event);
Event *popQueue(Queue* queue);
Queue *initializeQueue();
void pushPriorityQueue(Queue *eventQueue, Event *event);
void pushQueue(Queue *queue, Event *event);


#endif // MAINTEST