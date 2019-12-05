#include <iostream>
#include <algorithm>
#include <cstring>
#include <list>
#include <map>
#include <cstdio>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stddef.h>
#include <readline/readline.h> 
#include <readline/history.h> 
#include "disk.h"

using namespace std;

/*
#define BLOCKS 4096         // Number of blocks
#define BLOCK_SIZE 512      // Size of each block
#define MAX_ENTRIES 8       // Max number of entries per block
*/
#define BLOCKS 8192          //amount of data blocks reserved for file metadata
#define MAX_ENTRIES 8       // Max number of entries per block
#define BLOCK_SIZE 4096
#define MAX_NUM_FILES 256
// Indicate the status of the file entry
#define FREE 0
#define BUSY 1

//The number of file descriptors that can exist 64, 0-63 inclusive
#define FIRST_FD 0
#define LAST_FD 63
#define MAX_FDS 64

//The number of files that can be on the disk is 256, 0-255 inclusive 
#define FIRST_FILE_INDEX 0
#define LAST_FILE_INDEX 255

//cpp string, append to the string, call string temp, block_write(0,(cstring cast) temp)

// Entry in the file allocation table (FAT)
typedef struct FILE_ALLOCATION_ENTRY{
    short status;
    short next;
} fileEntry;

// FAT structure - 8192 number of fileEntry in the FAT
typedef struct FILE_ALLOCATION_TABLE{
    fileEntry file[BLOCKS];
} FAT;

// Entry in directory table structure - 64 byes each
typedef struct ENTRY{
    char fileName[15]; // 15 bytes (14 filename + terminator)
    unsigned short type; // 1 byte
    unsigned short fileSize; // 2 bytes
    unsigned short startingIndex; // 2 bytes
    unsigned short fileDes; //2 bytes
} Entry;

// Directory Entry Table. Each can hold up to 8 entries
typedef struct DIRECTORY_TABLE{
    Entry entry[MAX_ENTRIES];
} directory;

// Data block
typedef struct DATA_BLOCK{
    char sect[BLOCK_SIZE];
} datablock;

// DATA - all data blocks
typedef struct DATA_BLOCKS{
    datablock blocks[BLOCKS];
} DATA;










//function prototypes
int make_fs(char* disk_name);
int mount_fs(char* disk_name);
int unmount_fs(char* disk_name);
int fs_open(char* name);
int fs_close(int fildes);
int fs_create(char *name);
int fs_delete(char* name);
int fs_mkdir(char* name);
int fs_read(int fildes, void* buf, size_t nbyte);
int fs_write(int fildes, void* buf, size_t nbyte);
int fs_get_filesize(int fildes);
int fs_lseek(int fildes, off_t offset);
int fs_truncate(int fildes, off_t offset);

void create_fs(FAT *fat, Entry *root);


void updateValue(unsigned short *oldValue, unsigned short newValue);

short findFreeBlock(FAT * fat);
short findFreeEntry(directory *dir);
int findFileOffset(directory * dir, int file);
int findFileEntry(directory * dir, char * fileName);

void writeForFile(char *fileName);
void readFromFile(char *fileName);
//Shell functions
void shellPrompt(char *input);
char **shellParser (char *input);

int builtInCommands(char **input);

// Directory stack functions - navigating directory entries
int isEmpty();
int isFull();
int peek();
int pop();
void push(int dirBlock);
int changeDirectory(char *directoryName);

string tempStr;

// Directory stack
int directoryStack[BLOCKS];
int top = 0;
int topOfStack = 0;


FAT *fat;
DATA *Data;
Entry *root;

//FILE_DESCRIPTOR  *descriptors;

/*

block_write(1, "Test");
char *temp;
temp = (char *)malloc(1 * sizeof(char*));
block_read(temp);
printf("Here is the block_read: %s", temp);

*/

int make_fs(char *diskName){
    if(diskName != NULL){
        make_disk(diskName);
        open_disk(diskName);
    }
    else{
        return -1;
    }
    fat = (FAT*)malloc(sizeof(FAT));
    Data = (DATA*)malloc(sizeof(DATA));
    root = (Entry*)malloc(sizeof(Entry));
    
    fat->file[0].status = BUSY;
    fat->file[0].next = -1;

    updateValue(&(root->fileSize), 0);
    updateValue(&(root->startingIndex), 0);
    updateValue(&(root->type), 1);
    
    
    //char *bufferForRoot = (char*) calloc(BLOCK_SIZE, sizeof(char));
    strncpy(root->fileName, "/",sizeof(root->fileName));
    char *bufferForRoot = (char *)root;
    memcpy(bufferForRoot, root, sizeof(BLOCK_SIZE));

    block_write(0, bufferForRoot);
    int i;
    
    char *bufferForFat = (char*)fat;
    memcpy(bufferForFat, fat, sizeof(FAT));
    block_write(1, bufferForFat);

    free(bufferForRoot);
    free(bufferForFat);
    
    return 0;
}


// Currently unsuccessful
// Attempt to unmount the file system by storing the metadata to a string then later.
// The metadata information in the string is supposed to be split by 1 whitespace ' '.
// The entries of the directories were supposed to be split by the '-' character.
// Each entry and their respective metadata were supposed to be parsed back into memory once
// the mount_fs was called, unfortunately I didn't have time to complete the two functions.
// The end result for one entry was supposed to this: -file.txt
// Problem may also exist with how the structs are set up   
int unmount_fs(char *diskName){
    int i = 0;
    //if disk name was not found.
    if(diskName == NULL){
        puts("Could not find disk.");
        return -1;
    }

    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));
    // This was admittedly not a good way to find each entry file as it will 
    // loop through MAX_ENTRIES-times despite only testing with 1 file.
    for (i = 0; i < 1; i++){
        tempStr += '-';
        tempStr += dir1->entry->fileName;
        tempStr += '*';

        /*string fdStr = to_string(dir1->entry->fileDes);
        tempStr += fdStr;*/
        
        tempStr += '*';
        string sizeStr = to_string( dir1->entry->fileSize);
        tempStr += sizeStr;
        
        tempStr += '*';
        string indexStr = to_string(dir1->entry->startingIndex);
        tempStr += indexStr;
        
        tempStr += '*';
        string typeStr = to_string(dir1->entry->type);
        tempStr += typeStr;
    }
    return 0; 
}



void printStr(std::string temp){
    cout << temp << endl;
}

// Update METADATA
void updateValue(unsigned short *oldValue, unsigned short newValue){
    *oldValue = newValue;
}


short findFreeBlock(FAT *fat){
    short i;
    for (i = 1; i < BLOCKS; i++){
        if (fat->file[i].status == FREE){
            return i;
        }
    }
    return -1;
}


short findFreeEntry(directory *dir1){
    short i;
    for (i = 0; i < MAX_ENTRIES; i++){

        if (dir1->entry[i].startingIndex == 0){
            return i;
        }
    }
    return -1;
}

int findFileOffset(directory *dir1, int file){

    int size = dir1->entry[file].fileSize;
    int startIndex = dir1->entry[file].startingIndex;
    int count = 0;

    while (fat->file[startIndex].next != -1) {
        startIndex = fat->file[startIndex].next;
        count++;
    }

    return size - (BLOCK_SIZE * count);

}


int findFileEntry(directory * dir1, char * fileName){

    int i;
    for (i = 0; i < MAX_ENTRIES; i++){
        if ( ((strcmp(dir1->entry[i].fileName, fileName)) == 0)  && (dir1->entry[i].type == 0) && (dir1->entry[i].startingIndex != 0)){
            return i;
        }
    }
    return -1;
}



int fs_create(char *fileName){
    string strBuf;

    printf(" Creating file %s...\n", fileName);
    directory *dir1 = (directory *)malloc(sizeof(directory));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));

    short freeBlock = findFreeBlock(fat);
    fat->file[freeBlock].status = BUSY;
    fat->file[freeBlock].next = -1;


    short freeEntry = findFreeEntry(dir1);


    puts(" File's Metadata:");

    strncpy(dir1->entry[freeEntry].fileName, fileName, sizeof(fileName));
    printf(" File Name: %s \n", dir1->entry[freeEntry].fileName);
    
    updateValue(&dir1->entry[freeEntry].fileSize, 0);
    printf(" File Size: %d\n", dir1->entry[freeEntry].fileSize);
    
    updateValue(&dir1->entry[freeEntry].startingIndex, freeBlock);
    printf(" Starting index: %d\n", dir1->entry[freeEntry].startingIndex);

    updateValue(&dir1->entry[freeEntry].type, 0); //type is file

    memcpy(&(Data->blocks[peek()]), dir1, sizeof(*dir1));
    char *buf = (char *)dir1;
    block_write((int)freeBlock, buf);
    
    return 0;
}



int fs_mkdir(char * dirName){

    printf(" Creating directory: %s\n", dirName);
    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));

    short freeBlock = findFreeBlock(fat);
    fat->file[freeBlock].status = BUSY;
    fat->file[freeBlock].next = -1;

    short freeEntry = findFreeEntry(dir1);

    updateValue(&dir1->entry[freeEntry].fileSize, 0);
    updateValue(&dir1->entry[freeEntry].startingIndex, freeBlock);
    printf("\n\n\nStarting Index: %d\n\n\n", dir1->entry[freeEntry].startingIndex);
    updateValue(&dir1->entry[freeEntry].type, 1); //type is 1 which is directory
    
    
    strncpy(dir1->entry[freeBlock].fileName, dirName, sizeof(dirName));
    memcpy(&(Data->blocks[peek()]), dir1, sizeof(*dir1));
    char *buf = (char *)dir1;
    block_write((int)freeBlock, buf);
    return 0;
}



int fs_delete(char *fileName){

    printf(" Deleting directory %s...\n", fileName);
    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));


    int i = findFileEntry(dir1, fileName);
    if (i == -1){
        puts(" File not found");
        return -1;

    } 
    else {
        int block = dir1->entry[i].startingIndex;


        while (fat->file[block].next != -1){
            int temp = fat->file[block].next;
            fat->file[block].status = FREE;
            fat->file[block].next = -1;
            block = temp;
        }
        fat->file[block].status = FREE;
        fat->file[block].next = -1;

        updateValue(&dir1->entry[i].fileSize, 0);
        updateValue(&dir1->entry[i].startingIndex, 0);
    }

    memcpy(&(Data->blocks[peek()]), dir1, sizeof(*dir1));
    return 0;
}


int fs_read(int filedes,void *buf,size_t nbytes){

    //printf(" Reading file %s\n", fileName);
    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));


    //int file = findFileEntry(dir1, fileName);
    if (filedes == -1){
        printf(" ERROR: File not found \n");

    } 
    else {
        char tempBuf[BLOCK_SIZE + 1];
        char buf[dir1->entry[filedes].fileSize + 1];
        memset(buf, '\0', sizeof(buf));

        int startBlock = dir1->entry[filedes].startingIndex;
        


        while(fat->file[startBlock].next != -1){
            memset(tempBuf, '\0', sizeof(tempBuf));
            memcpy(tempBuf, Data->blocks[startBlock].sect, BLOCK_SIZE);
            strcat(buf, tempBuf);
            startBlock = fat->file[startBlock].next;
        }

        int offset = findFileOffset(dir1, filedes);
        memset(tempBuf, '\0', sizeof(tempBuf));
        memcpy(tempBuf, Data->blocks[startBlock].sect, offset);
        strcat(buf, tempBuf);

        block_read(startBlock, buf);
        printf("%s\n", buf);
    }
    memcpy(&(Data->blocks[peek()]), dir1, sizeof(*dir1));
    return 0;
}

void readFromFile(char *fileName){
    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));
    int filedes = findFileEntry(dir1, fileName);
    
    char buf[1024] = "";
    
    size_t len = strlen(buf);
    fs_read(filedes, buf, len);
}


// WRITE DATA TO FILE
int fs_write(int filedes, void* buf, size_t nbyte){

    //printf(" Writing to file %s...\n", fileName);
    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));


    //int file = findFileEntry(dir1, fileName);

    if (filedes == -1){

        printf(" ERROR: File not found. \n");
        return -1;

    } 
    else{

        int startingBlock = dir1->entry[filedes].startingIndex;
        while (fat->file[startingBlock].next != -1){

            startingBlock = fat->file[startingBlock].next;
        }

        int bufferLength = strlen((char*)buf);
        while (bufferLength > 0){
            printf(" Starting to write at block: %d\n", startingBlock);
            printf(" Buffer Length: %d\n", bufferLength);
            
            int fileOffset = findFileOffset(dir1, filedes);
            printf(" Offset: %d\n", fileOffset);

            //int n = (bufLength < (BLOCK_SIZE - offset)) ? bufLength : BLOCK_SIZE - offset;
            int blockNum;
            if(bufferLength < (BLOCK_SIZE - fileOffset)){
                blockNum = bufferLength;
            }
            else{
                blockNum = BLOCK_SIZE - fileOffset;
            }
            printf(" Block Number: %d\n", blockNum);

            strncpy(Data->blocks[startingBlock].sect + fileOffset, (char *)buf + (strlen((char*)buf) - bufferLength), blockNum);
            
            updateValue(&dir1->entry[filedes].fileSize, blockNum + dir1->entry[filedes].fileSize);
            
            printf(" Filesize: %d\n", dir1->entry[filedes].fileSize);
            bufferLength = bufferLength - blockNum;

            if ((fileOffset + blockNum) == BLOCK_SIZE){

                int freeBlock = findFreeBlock(fat);
                fat->file[startingBlock].next = freeBlock;
                fat->file[freeBlock].status = BUSY;
                fat->file[freeBlock].next = -1;
                startingBlock = freeBlock;
            }
        }
    }
    memcpy(&(Data->blocks[peek()]), dir1, sizeof(*dir1));
    return 0;
}



void writeForFile(char *fileName){
    
    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));
    int filedes = findFileEntry(dir1, fileName);
    
    char buf[1024] = "";
    printf("\nWrite to a file: ");
    scanf("%s", buf);
    size_t len = strlen(buf);
    fs_write(filedes, buf, len);
}


int peek(){
   return directoryStack[topOfStack];
}

int pop(){
    int info;

    if (topOfStack == 0) {
        return 0;
    } 
    else {
        info = directoryStack[topOfStack];
        topOfStack = topOfStack - 1;
        return info;
    }
}

void push(int directoryBlock){

    if ( topOfStack == BLOCKS) {
        printf(" Stack is full! \n");

    } 
    else {
        topOfStack = topOfStack + 1;
        directoryStack[top] = directoryBlock;
    }
}

//Swith between directories in the file system just like a regular linux terminal.
//In order to do so in the file system, the directory stack helps changing to a 
//new directory by pushing an element's startingIndex from the directory entry table. 
int changeDirectory(char *directoryName){

    if (strcmp(directoryName, "..") == 0){
        pop();
        return 1;
    }
    
    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));


    for (int i = 0; i < MAX_ENTRIES; i++){
        if ((strcmp(directoryName, dir1->entry[i].fileName) == 0) && (dir1->entry[i].type == 1)){
            push(dir1->entry[i].startingIndex);
            return 1;
        }
    }
    return 0;
}



char **shellParser (char *input){ 
    int bufferSize = 1024, index = 0; //for the string token position in tokens [index] 
    char **tokens = (char **)malloc(bufferSize * sizeof(char *)); 
    // allocating space for the tokens and its double pointers because elements in the array are  
    // pointers, the array must be a pointer to a pointer 
    char *token; // represents a token in the array of tokens  
    token = strtok(input, " \t\n"); // This  -->  " \t\n" represents the whitespaces that are common 
    while(token != NULL) {  
        tokens [index] = token; 
        // Maybe check ifthere needs to be a reallocation ifthe bufferSize somehow is less than index. 
        token = strtok(NULL, " \t\n"); // place null terminated \0 at the end of each token 
        index++; 
    } 
    tokens[index] = NULL;
    return tokens; 
}


int builtInCommands(char **parsedArgs){ 
    if(parsedArgs[0] == NULL){  
        // if the user doesn’t input a command that are built-in or external i.e. fileDescriptorfileDescriptor or \n 
        puts("Command doesn’t exit. Try again."); 
        return 0;   
    } 
    if(strcmp (parsedArgs[0], "quit")==0){ 
        puts("Exiting the file system.");
        exit(0);
    } 
    else if(strcmp (parsedArgs[0], "cd")==0){ 
        changeDirectory(parsedArgs[1]); 
        return 0; 
    }
    else if(strcmp(parsedArgs[0], "fs_create")==0){
        //createFile(fat, Data, parsedArgs[1]);
        fs_create(parsedArgs[1]);
    }
    else if(strcmp(parsedArgs[0], "fs_write") == 0){
        //char **extStr = fileParser(parsedArgs[1]);
        writeForFile(parsedArgs[1]);
    }
    else if(strcmp(parsedArgs[0], "fs_read") == 0){
        //char **extStr = fileParser(parsedArgs[1]);
        readFromFile(parsedArgs[1]);
    }
    else if(strcmp(parsedArgs[0], "fs_delete") == 0){
        //char **extStr = fileParser(parsedArgs[1]);
        fs_delete(parsedArgs[1]);
    }
    else if(strcmp(parsedArgs[0], "fs_mkdir") == 0){
        fs_mkdir(parsedArgs[1]);
    }
    else if(strcmp(parsedArgs[0], "unmount_fs")){
        unmount_fs(parsedArgs[0]);
    }
    else if(strcmp(parsedArgs[0], "printStr")){
        printStr(tempStr);
    }
    /*else if(strcmp(parsedArgs[0], "mount_fs")){
        mount_fs(parsedArgs[1]);
    }*/
    
    // if the function fails, return -1 
    return -1; 
}

void shellPrompt(char *input){
    char cwd[1024]; //current working directory
    getcwd(cwd, sizeof(cwd));
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        printf("%s>> ",cwd);
        fgets(input, 1024, stdin);
        
    }
}
















int main(){
    char diskName[15] = "DISK";
    
    puts("\n------MY FILE SYSTEM------\n");

    directoryStack[top] = 0;

    make_fs(diskName);

    /*char input[1024];
    int exitFlag = 0;
    shellPrompt(input);
    int count = 0;
    while(1){
        char **args = shellParser(input);
        //exitFlag = checkAndExecute(args, data, fat);
        builtInCommands(args);
        free(args);
        shellPrompt(input);
    }*/
    char dirName[15] = "dir1";
    fs_mkdir(dirName);
    changeDirectory(dirName);
    char fileName[15] = "file.txt";
    //createFile(fat, Data, fileName);
    fs_create(fileName);
    //char buf[6] = "Hello";
    //writeFile(fat, Data, fileName, buf);
    writeForFile(fileName);
    readFromFile(fileName);
    unmount_fs(diskName);
    printStr(tempStr);
    return 0;
}