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
#define NUM_WORKERS 1

std::vector<std::string> openDictionary(std::string filename);
void checkWord(std::string word, std::vector<std::string> & listOfStr);
void *workerThread(void *args);



queue <int> connection; //file descriptors 
queue <std::string> log;

pthread_mutex_t connectionMutex;
pthread_mutex_t logMutex;

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




int main(){
	std::vector<std::string> list;
	std::string file = "dictionary.txt";
	list = openDictionary(file);
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
	cout << "Before thread\n" << endl;
	/*pthread_t workers[NUM_WORKERS];
	for(int i = 0; i < NUM_WORKERS; i++){
		pthread_create(&workers[i], NULL, &workerThread,NULL);
		pthread_join(&workers[i], NULL);
	}*/
	pthread_t thread_id;
    pthread_create(&thread_id, NULL, &workerThread, NULL);
    pthread_join(thread_id, NULL);
	cout << "\nAfter thread" <<endl;
	return 0;
}



