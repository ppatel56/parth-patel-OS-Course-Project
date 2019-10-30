/*
*Spell checker using a dictionary. 
*The Client-side would send a word and the Server-side 
will recieve that word and check to see if the word is spelled correctly
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIST_LEN 99171

// create a buffer for clients' messages to be stored in waiting to be picked by 
// a working thread to check if the word sent was spelled correctly or not 
// then send it to the log file 
void createFIFOQueue();  
void **checkWord(char *string, char **dictionaryList);
char **openDictionary(char *file);


char **openDictionary(char *file){
    FILE *dictionaryFile = fopen(file, "r");
    puts("openDic 1");
    if(dictionaryFile == NULL){
        puts("File is not found");
        exit(1);
    }
    puts("openDic 2");
    int index = 0;
    char **wordList = malloc(LIST_LEN *sizeof(char *) + 1);
    
    if(wordList == NULL){
       exit(1); 
    }
    puts("openDic 3");
    char line[50];
    puts("openDic 4");
    while((fgets(line, 50, dictionaryFile)) != NULL){
        puts("Loop");
        wordList[index] = (char *) malloc(strlen(line) * sizeof(char *) + 1);
        if(wordList[index] == NULL){
            puts("Unable to allocate memory.\n");
            exit(1);
        }
        int temp = strlen(line) - 2;
        line[temp] = '\0';
        puts("openDic 5");
        strcpy(wordList[index],line);
        index++;
    }
    puts("openDic 6");
    fclose(dictionaryFile);
    return wordList;
}


void **checkWord(char *string, char **dictionaryList){
    for(int i = 0; i < LIST_LEN; i++){
        if(strcmp(string,dictionaryList[i])==0){
            puts("Word is Correct\n");
            break;
        }
        else if(strcmp(string, dictionaryList[i])!=0 && NULL){
            puts("Word is incorrect\n");
        }
    }
}


int main(){
    //printf("Hello, World!\n");
    puts("main 1");
    char word[50] = "vase";
    char *file = "dictionary.txt";
    puts("main 2");
    char **wordList = openDictionary(file);
    puts("main 3");
    checkWord(word, wordList);
    puts("main 4");
    puts("Success!");
    return 0;
}