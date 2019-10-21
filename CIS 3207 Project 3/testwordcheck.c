/*
*Spell checker using a dictionary. 
*The Client-side would send a word and the Server-side 
will recieve that word and check to see if the word is spelled correctly
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

void clientSide();

void serverSide();

// create a buffer for clients' messages to be stored in waiting to be picked by 
// a working thread to check if the word sent was spelled correctly or not 
// then send it to the log file 
void createFIFOQueue();  


int main(){
    printf("Hello, World!\n");
    return 0;
}