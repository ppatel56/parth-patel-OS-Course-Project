The program was a event simulator which simulates the event processes of a single CPU and two Disks. 
The simulation of the three devices requires four queues, three FIFO queues and one priority queue.
The three FIFO queues are CPU Queue, Disk 1 Queue, Disk 2 Queue; the priority queue in my program is dubbed the Event Queue.
The Event Queue puts emphasis on an event with the least time to have greater priority than events with larger amount of time.
Then theres the Event Handler function which as the name suggests, handles all of the events when an event goes in from the main while loop, which basically removes an event from Event Queue and places it in event handler.
  