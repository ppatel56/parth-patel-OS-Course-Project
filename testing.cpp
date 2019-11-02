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

#define LIST_LEN 99171
#define PORT_NUM 8888
#define NUM_WORKERS 4

std::vector<std::string> openDictionary(std::string filename);
void checkWord(std::string word, std::vector<std::string> & listOfStr);
void *workerThread(void *args);

std::vector<std::string> list; //contains the dictionary words

queue <int> connection; //file descriptors 
queue <std::string> log;

pthread_mutex_t connectionQueueLock;
pthread_mutex_t logQueueLock;
pthread_mutex_t logLock;

pthread_cond_t connectionEmpty; //connection queue
pthread_cond_t connectionFull; //connection queue

pthread_cond_t logEmpty; //log queue
pthread_cond_t logFull; //log queue

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

void checkWord(std::string word, std::vector<std::string> & listOfStr){
    //char *result = "incorrect";
	std::string result = "incorrect";
    for (int i = 0; i < LIST_LEN; i++) {
      if(word == listOfStr[i]) {
          result = "correct";
          break;
      }
    }
    //printf("%s is %s spelling.\n", word, result);
	cout << word << " is " << result << " spelling." << endl;
}
 

void *workerThread(void *args){
	sleep(1);
	cout << "Hello, World" << endl;
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
	//Setting up the client sockets 
	struct sockaddr_in client;
	int clientLen = sizeof(client);
	int connectionSocket, clientSocket, bytesReturned;

	//Initialize the locks and condition variables in order to 
	//be used in the threads

	pthread_mutex_init(&connectionQueueLock, NULL);
	pthread_mutex_init(&logQueueLock, NULL);
	pthread_mutex_init(&logLock, NULL);

	pthread_cond_init(&connectionEmpty, NULL);
	pthread_cond_init(&connectionFull, NULL);

	pthread_cond_init(&logEmpty, NULL);
	pthread_cond_init(&logFull, NULL);
	
	pthread_t workers[NUM_WORKERS]

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
	
	return 0;
}



/*
int main(int argc, char **argv){
    queue<int> myQ;
    for(int i = 0; i<10; i++){
        cout <<"myQ " << i << endl;
        myQ.push(i); 
    }

    string str = "Hello World!\n";
    string result = hi(str);
    return 0;
}
*/



/*
void *myThread(void *vargp){
    sleep(1);
    printf("Hello There!\n");
    return NULL;
}

int main(){

    printf("Before thread\n");
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, myThread, NULL);
    pthread_join(thread_id, NULL);
    printf("After thread\n");
    return 0;
}
*/

// Get the contents of file in a vector
	/*
	while(1) {
		std::string word = "";
		cout <<"Enter a word: ";
		//scanf("%s", word);
		cin >> word;
		if(word == "quit"){
			cout << "Quiting program..." << endl;
			break;
		}
		checkWord(word, list);
  	}
	*/
	/*cout << "Before thread\n" << endl;
	
	pthread_t workers[NUM_WORKERS];
	for(int i = 0; i < NUM_WORKERS; i++){
		pthread_create(&workers[i], NULL, &workerThread,NULL);
		pthread_join(&workers[i], NULL);
	}
	pthread_t thread_id;
    pthread_create(&thread_id, NULL, &workerThread, NULL);
    pthread_join(thread_id, NULL);
	cout << "\nAfter thread" <<endl;*/