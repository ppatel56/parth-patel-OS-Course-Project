using namespace std;
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <unistd.h> 
#include <stdio.h>  
#include <stdlib.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <pthread.h>
#include <string.h>

#define LIST_LEN 99171
#define PORT_NUM 8888
#define NUM_WORKERS 4
#define MAX_SIZE 10
#define BUF_LEN 512

std::vector<std::string> openDictionary(std::string filename);
std::string checkWord(std::string word, std::vector<std::string> & listOfStr);
void *workerThread(void *args);

std::vector<std::string> list; //contains the dictionary words

queue <int> connectionQueue; //queue of socket descriptors 
queue <std::string> logQueue; //queue of logs (messages/strings)

pthread_mutex_t connectionQueueLock;
pthread_mutex_t logQueueLock;
pthread_mutex_t logLock;

pthread_cond_t connectionEmpty; //connection queue
pthread_cond_t connectionFill; //connection queue

pthread_cond_t logEmpty; //log queue
pthread_cond_t logFill; //log queue








/*
 * It will iterate through all the lines in file and
 * put them in given vector
 */

std::vector<std::string> openDictionary(std::string fileName){
	std::vector<std::string> listOfStr; 
	// Open the File
	std::ifstream file(fileName.c_str());
 
	// Check if object is valid
	if(!file){
		std::cerr << "Cannot open the File : "<< fileName <<std::endl;
		//return false;
		exit(1);
	}
 
	std::string str;
	// Read the next line from File untill it reaches the end.
	while (std::getline(file, str)){

		// Line contains string of length > 0 then save it in vector
		if(str.size() > 0)
			listOfStr.push_back(str);
	}
	//Close The File
	file.close();
	//return true;
	return listOfStr;
}

std::string checkWord(std::string word, std::vector<std::string> &listOfStr){
    //char *result = "incorrect";
	std::string result = " is incorrect spelling.";
    for (int i = 0; i < LIST_LEN; i++) {
      if(word == listOfStr[i]) {
          result = " is correct spelling.";
          break;
      }
    }
    
	word = word.append(result);
	return word;
}
 

void *workerThread(void *args){
	//sleep(1);
	//cout << "Hello, World" << endl;
	char *promptMessage = "Enter word to spell check >> ";
	char *errorMessage = "Unable to display message.\n";
	char *exitMessage = "Connection with server is closed.\n"; //if the client types 0

	//Main while loop for threads as their is constant locking\unlocking, conditional variables, and critical section
	while(1){
		pthread_mutex_lock(&connectionQueueLock);
		if(connectionQueue.empty()){
			pthread_cond_wait(&connectionFill, &connectionQueueLock);
		}
		//Get the client socket descriptor
		int clientSocket = connectionQueue.front();
		connectionQueue.pop();
		//Send signal and release the lock on Connection queue
		pthread_mutex_unlock(&connectionQueueLock);
		pthread_cond_signal(&connectionEmpty);

		while(1){
			//string str = "some string";
			//char *cstr = &str[0]
			char recvBuffer[BUF_LEN] = "";
			send(clientSocket, promptMessage, strlen(promptMessage), 0);
			int bytesReturned;
			bytesReturned = recv(clientSocket, recvBuffer, BUF_LEN, 0);
			
			if(bytesReturned <= -1){
				send(clientSocket, errorMessage, strlen(errorMessage),0);
				continue;
			}
			else if(atoi(&recvBuffer[0]) == -1){
				send(clientSocket, exitMessage, strlen(exitMessage), 0);
				close(clientSocket);
				break;
			}
			else{

				//check word spelling
				//combine word and result
				//send() to client the result
				//struct sockaddir_in client = job->client
				//mutex_lock(&logLock)
				//check if log queue is full
				//push log onto log queue
				//signal(empty)
				//mutex_unlock(&logLock)
				recvBuffer[strlen(recvBuffer)-1] = '\0';
				recvBuffer[bytesReturned-2] = '\0';
				string word(recvBuffer);
				word = checkWord(word, list);
				
				pthread_mutex_lock(&logQueueLock);
				if(logQueue.size() >= MAX_SIZE){
					pthread_cond_wait(&logEmpty, &logQueueLock);
				}
				//Add the word to the log queue
				logQueue.push(word);
				//signal to logThread() function
				
				pthread_mutex_unlock(&logQueueLock);
				pthread_cond_signal(&logFill);
			}
		}
	}
}


void *logThread(void *args){
	ofstream logFile;
	logFile.open("log.txt");
	while(1){
		pthread_mutex_lock(&logQueueLock);
		if(logQueue.empty()){
			pthread_cond_wait(&logFill, &logQueueLock);
		}
		std::string logStr = logQueue.front();
		logQueue.pop();
		pthread_cond_signal(&logEmpty);
		pthread_mutex_unlock(&logQueueLock);

		//lock the log file
		//pthread_mutex_lock(&logLock);
		//ofstream logFile;
		//logFile.open("log.txt");
		//std::ofstream file(logFile.c_str());
		logFile << logStr;
		//logFile.close();

		//unlock the log file
		//pthread_mutex_unlock(&logLock);
	}
	logFile.close();
}




//Given function in the textbook
int open_listenfd(int port){
	int listenfd, optval=1;
	struct sockaddr_in serveraddr;

	/* Create a socket descriptor */
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		return -1;
	}

	 /* Eliminates "Address already in use" error from bind */
	 if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval , sizeof(int)) < 0){
	 	return -1;
	 }

	 //Reset the serveraddr struct, setting all of it's bytes to zero.
	 //Some properties are then set for the struct, you don't
	 //need to worry about these. 
	 //bind() is then called, associating the port number with the
	 //socket descriptor.
	 bzero((char *) &serveraddr, sizeof(serveraddr));
	 serveraddr.sin_family = AF_INET;
	 serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	 serveraddr.sin_port = htons((unsigned short)port);
	 if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0){
	 	return -1;
	 }

	 //Prepare the socket to allow accept() calls. The value 20 is 
	 //the backlog, this is the maximum number of connections that will be placed
	 //on queue until accept() is called again. 
	 if (listen(listenfd, 20) < 0){
	 	return -1;
	 }

	 return listenfd;
}






int main(int argc, char **argv){

	int connectionPort;	
	std::string file = "dictionary.txt";
	list = openDictionary(file);

	//Initialize the locks and condition variables in order to 
	//be used in the threads

	pthread_mutex_init(&connectionQueueLock, NULL);
	pthread_mutex_init(&logQueueLock, NULL);
	pthread_mutex_init(&logLock, NULL);

	pthread_cond_init(&connectionEmpty, NULL);
	pthread_cond_init(&connectionFill, NULL);

	pthread_cond_init(&logEmpty, NULL);
	pthread_cond_init(&logFill, NULL);

	if(argc == 1){
		connectionPort = PORT_NUM;
		file;
	}
	else if(argc == 2){
		connectionPort = atoi(argv[1]);
		file;
	}
	else{
		connectionPort = atoi(argv[1]);
		file = argv[2];
	}
	//We can't use ports below 1024 and ports above 65535 don't exist.
	if(connectionPort < 1024 || connectionPort > 65535){
		printf("Port number is either too low(below 1024), or too high(above 65535).\n");
		return -1;
	}

	//Create worker threads
	pthread_t workers[NUM_WORKERS];
	for(int i = 0; i<NUM_WORKERS; i++){
		pthread_create(&workers[i], NULL, &workerThread, NULL);
	}

	//create log thread
	pthread_t log;
	pthread_create(&log, NULL, &logThread, NULL);
	//messages for when connection is successful and when connection queue is full
	char *connectionSuccess = "Connection is successful. Type \"-1\" to exit\n";
	char *connectionQueueFull = "Connection Queue is full\n";

	//Setting up the client sockets 
	struct sockaddr_in client;
	//int clientLen = sizeof(client);
	int connectionSocket, clientSocket;
	connectionSocket = open_listenfd(connectionPort);
	socklen_t clientLen = sizeof(client);
	//Main while loop for storing socket descriptors
	while(1){
		clientSocket = accept(connectionSocket, (struct sockaddr*)&client, &clientLen);
		if(clientSocket == -1){
			cout << "Failed to connect to client socket." << endl;
			continue;
		}

		pthread_mutex_lock(&connectionQueueLock);
		if(connectionQueue.size() >= MAX_SIZE){
			send(clientSocket, connectionQueueFull, strlen(connectionQueueFull),0);
			pthread_cond_wait(&connectionEmpty, &connectionQueueLock);
		}
		//Otherwise
		cout << "Connected to a new client. Client ID: " << clientSocket << endl;
		
		send(clientSocket, connectionSuccess, strlen(connectionSuccess), 0);

		//push client socket descriptor to the connection queue
		connectionQueue.push(clientSocket);

		//Signal the consumer which is in the workerThread()
		pthread_cond_signal(&connectionFill);
		//Unlock the critical section
		pthread_mutex_unlock(&connectionQueueLock);
		
		//pthread_cond_signal(&connectionFill);


	}


	return 0;
}