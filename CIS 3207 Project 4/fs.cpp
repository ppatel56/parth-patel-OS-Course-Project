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


#define BLOCKS 8192          //amount of data blocks reserved for file metadata
#define MAX_ENTRIES 8       // Max number of entries per block
#define BLOCK_SIZE 4096
#define MAX_NUM_FILES 256
// Indicate the status of the file entry
#define EMPTY 0
#define BUSY 1

//The number of file descriptors that can exist 64, 0-63 inclusive
#define FIRST_FD 0
#define LAST_FD 63
#define MAX_FDS 64

//The number of files that can be on the disk is 256, 0-255 inclusive 
#define FIRST_FILE_INDEX 0
#define LAST_FILE_INDEX 255

#define FILE_TYPE 0
#define DIR_TYPE 1
//cpp string, append to the string, call string temp, block_write(0,(cstring cast) temp)


// The short data type is used in order conserve memory

// Entry in the FAT
typedef struct{
    short status; //status of the block, EMPTY or BUSY
    short nextBlock; //next block of the file entry
} fileEntry;

// FAT structure - 8192 number of fileEntry in the FAT
typedef struct{
    fileEntry file[BLOCKS];
} FAT;

// Entry in directory table structure 
typedef struct {
    char fileName[15]; // 15 bytes (14 filename + terminator)
    unsigned short beginningIndex; // index of the file
    unsigned short type; // determines if the entry is a file (0) or directory (1)
    unsigned short fileSize; // size of the file
    unsigned short fileDes; // file descriptor
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


//Helper functions 
void updateValue(unsigned short *oldValue, unsigned short newValue);

short findAvailableBlock(FAT *fat);
short findAvailableEntry(directory *dir);
int findFileOffset(directory *dir, int file);
int findFileEntry(char *fileName, directory *dir);

void writeForFile(char *fileName);
void readFromFile(char *fileName);
//Shell functions
void shellPrompt(char *input);
char **shellParser (char *input);
char **pathParser(char *input);
int builtInCommands(char **input);

// Directory stack functions - navigating directory entries
int isEmpty();
int isFull();
int peek();
int pop();
void push(int directoryBlock);
void printStack();
int getNumEntries();
int changeDirectory(char *directoryName);


//used for the unmount process, it is where the entries are stored
//in order to be parsed back by the mount_fs
string tempStr;

// Directory stack - where the directories will stored in order to change directories  
int directoryStack[MAX_NUM_FILES]; //since directories are technically file
int topOfStack = 0;

int fileDescriptor;

FAT *fat;
DATA *Data;
Entry *root;



/*
TEST FOR BLOCK_READ AND BLOCK_WRITE FUNCTIONS
block_write(1, "Test");
char *temp;
temp = (char *)malloc(1 * sizeof(char*));
block_read(temp);
printf("Here is the block_read: %s", temp);

*/


/*In the make_fs() function, the diskName is passed in then is checked if the name is valid
* then tries to make_disk and open_disk otherwise return -1.
* The first entry in the fat is busy since the root directory is essentially in the block.
* The root directory is then given the appropriate metadata.
* The memcpy is used to get the address of root to a buffer, then buffer is written to block 0.
* The memcpy is used once again to get addeesss of fat to a buffer, then buffer is written to block 1.
* Both buffers are then freed.
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
    fat->file[0].nextBlock = -1;

    updateValue(&(root->fileSize), 0);
    updateValue(&(root->beginningIndex), 0);
    updateValue(&(root->type), DIR_TYPE);
    
    
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


/*Currently unsuccessful
* Attempt to unmount the file system by storing the metadata to a string then later.
* The metadata information in the string is supposed to be split by 1 whitespace ' '.
* The entries of the directories were supposed to be split by the '-' character.
* Each entry and their respective metadata were supposed to be parsed back into memory once
* the mount_fs was called, unfortunately I didn't have time to complete the two functions.
* The end result for one entry was supposed to this: -file.txt
* Problem may also exist with how the structs are set up
*/   
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
    for (i = 0; i < MAX_ENTRIES; i++){
        tempStr += '-';
        tempStr += dir1->entry->fileName;
        tempStr += '*';

        /*string fdStr = to_string(dir1->entry->fileDes);
        tempStr += fdStr;*/
        
        tempStr += '*';
        string sizeStr = to_string( dir1->entry->fileSize);
        tempStr += sizeStr;
        
        tempStr += '*';
        string indexStr = to_string(dir1->entry->beginningIndex);
        tempStr += indexStr;
        
        tempStr += '*';
        string typeStr = to_string(dir1->entry->type);
        tempStr += typeStr;
    }
    return 0; 
}


// print out the the string that holds the entries and their metadata
void printStr(std::string temp){
    cout << temp << endl;
}

// Update the metadata for the entries
void updateValue(unsigned short *oldValue, unsigned short newValue){
    *oldValue = newValue;
}

// Find the available block available in the FAT so that the free block
short findAvailableBlock(FAT *fat){
    short i;
    for (i = 1; i < BLOCKS; i++){
        if (fat->file[i].status == EMPTY){
            return i;
        }
    }
    return -1;
}

// Find the next available entry in the directory 
short findAvailableEntry(directory *dir1){
    short i;
    for (i = 0; i < MAX_ENTRIES; i++){
        if (dir1->entry[i].beginningIndex == 0){
            return i;
        }
    }
    return -1;
}

/*Finding the file offset by getting the file size and file's index.
* By iterating through the FAT using the file's index till it reaches 
* the next block value of -1.
* Then the offset of file's size - (BLOCK_SIZE*count) is returned.
*/
int findFileOffset(directory *dir1, int file){

    int sizeOfFile = dir1->entry[file].fileSize;
    int startIndex = dir1->entry[file].beginningIndex;
    int count = 0;

    while (fat->file[startIndex].nextBlock != -1) {
        startIndex = fat->file[startIndex].nextBlock;
        count++;
    }

    return sizeOfFile-(BLOCK_SIZE*count);
}

// To find a file entry, the function iterates through the the directory till MAX_ENTRIES.
// Once the name, type, and index of directory entry matches the file name, type file, and non-zero index;
// return that index
int findFileEntry(char *fileName, directory *dir1){

    int index;
    for (index = 0; index < MAX_ENTRIES; index++){
        if ( ((strcmp(dir1->entry[index].fileName, fileName)) == 0)  && (dir1->entry[index].type == FILE_TYPE) && (dir1->entry[index].beginningIndex != 0)){
            return index;
        }
    }
    return -1;
}


/*The fs_create function creates a file. 
* The directory is first initialized, then get metadata from the data block using memcpy.
* The metadata's address is copied into directory's address.
* Then find the next available block for the FAT and then initialize FAT.
* Then put in each respective metadata information into the directory entry (file).
* Then memcpy the directory into the data block, then block_write using 
* the available block from the FAT with the buffer being the directory.
*/
int fs_create(char *fileName){
    string strBuf;

    directory *dir1 = (directory *)malloc(sizeof(directory));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));

    short freeBlock = findAvailableBlock(fat);
    fat->file[freeBlock].status = BUSY;
    fat->file[freeBlock].nextBlock = -1;

    short freeEntry = findAvailableEntry(dir1);

    puts(" File's Metadata:");

    strncpy(dir1->entry[freeEntry].fileName, fileName, sizeof(fileName));
    printf(" File Name: %s \n", dir1->entry[freeEntry].fileName);
    
    updateValue(&dir1->entry[freeEntry].fileSize, 0);
    printf(" File Size: %d\n", dir1->entry[freeEntry].fileSize);
    
    updateValue(&dir1->entry[freeEntry].beginningIndex, freeBlock);
    printf(" Beginning index: %d\n", dir1->entry[freeEntry].beginningIndex);

    updateValue(&dir1->entry[freeEntry].type, FILE_TYPE);
    printf(" File Type: %d\n", dir1->entry[freeEntry].type);

    updateValue(&dir1->entry[freeEntry].fileDes, -1);
    printf(" File Descriptor: %d\n", dir1->entry[freeEntry].fileDes);
    memcpy(&(Data->blocks[peek()]), dir1, sizeof(*dir1));
    char *buf = (char *)dir1;
    block_write((int)freeBlock, buf);
    
    printf(" Created file %s\n", fileName);
    return 0;
}

/*The fs_mkdir function is essentially the same function as fs_create function with the exception of the types
*/
int fs_mkdir(char * dirName){

    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));

    short freeBlock = findAvailableBlock(fat);
    fat->file[freeBlock].status = BUSY;
    fat->file[freeBlock].nextBlock = -1;

    short freeEntry = findAvailableEntry(dir1);
    
    updateValue(&dir1->entry[freeEntry].fileSize, 0);
    printf(" Directory Size: %d\n", dir1->entry[freeEntry].fileSize);
    updateValue(&dir1->entry[freeEntry].beginningIndex, freeBlock);
    printf(" Starting Index: %d\n", dir1->entry[freeEntry].beginningIndex);
    updateValue(&dir1->entry[freeEntry].type, DIR_TYPE); //type is 1 which is directory
    printf(" Dir Type: %d\n", dir1->entry[freeEntry].type);
    
    strncpy(dir1->entry[freeBlock].fileName, dirName, sizeof(dirName));
    printf(" Dir Name: %s\n", dir1->entry[freeBlock].fileName);
    memcpy(&(Data->blocks[peek()]), dir1, sizeof(*dir1));
    char *buf = (char *)dir1;
    block_write((int)freeBlock, buf);
    printf(" Creating directory: %s\n", dirName);
    return 0;
}

/*Check if the file name is invalid and return -1 if it is.
* Otherwise, set up the directory like usual then iterate through maximum number
* of file descriptors till there is one available then if directory entry is doesn't
* have a filedes then assign the file descriptor to it and return it.
* 
*/
int fs_open(char *fileName){
    
    if(fileName == NULL){
        return -1;
    }
    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));

    int i;
    for(i = 0; i< MAX_FDS; i++){
        if(dir1->entry->fileDes == -1 && strcmp(dir1->entry->fileName, fileName) == 0){
            dir1->entry->fileDes = i;
            memcpy(&(Data->blocks[peek()]), dir1, sizeof(*dir1));
            return dir1->entry->fileDes;
        }
    }

    return -1;
}
/*Basically do the opposite of the fs_open function by setting the directory entry's 
* fileDes to -1 and thus closing the file
*/
int fs_close(int filedes){
    if(filedes < FIRST_FD || filedes >LAST_FD){
        return -1;
    }
    
    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));

    int i;
    for(i = 0; i< MAX_FDS; i++){
        if(dir1->entry->fileDes != -1){
            dir1->entry->fileDes = -1;
            memcpy(&(Data->blocks[peek()]), dir1, sizeof(*dir1));
        }
    }
    return 0;
}

//Just set up the directory entry then return the fileDes if it matches the
//parameter.
int fs_get_fileSize(int filedes){
    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));
    if(dir1->entry->fileDes== filedes){
        printf("fileSize: %d\n", dir1->entry->fileSize);
        return 0;
    }
    return -1;
}

/*The set up the directory entry per usual then find the file entry using the helper function
* The index should be greater than -1, then get dir entry's index and passed it to the FAT.
* While FAT has not reached -1, make each block in FAT empty.
* Then update the dir entry to 0 in size and index 
*/
int fs_delete(char *fileName){

    printf(" Deleting directory %s\n", fileName);
    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));

    int index = findFileEntry(fileName, dir1);
    if (index == -1){
        puts(" File not found");
        return -1;

    } 
    else{
        int blockIndex = dir1->entry[index].beginningIndex;


        while (fat->file[blockIndex].nextBlock != -1){
            int temp = fat->file[blockIndex].nextBlock;
            fat->file[blockIndex].status = EMPTY;
            fat->file[blockIndex].nextBlock = -1;
            blockIndex = temp;
        }
        fat->file[blockIndex].status = EMPTY;
        fat->file[blockIndex].nextBlock = -1;

        updateValue(&dir1->entry[index].fileSize, 0);
        updateValue(&dir1->entry[index].beginningIndex, 0);
    }

    memcpy(&(Data->blocks[peek()]), dir1, sizeof(*dir1));
    return 0;
}

/*Set up the directory entry as usual then check if the filedes is invalid.
* Get the dir entry's index then iterate through the FAT till it reaches -1 and make 
* the next block the index each iteration.
* Then get the length of buffer that was passed in.
* While the buffer length is greater than 0, get the offset of the file and block number
* Copy the buffer into the data blocks using strncpy.
* Then find the free block and use it to do block_write into the disk the string
* Then initialize the FAT
*/
int fs_write(int filedes, void* buf, size_t nbyte){
    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));


    if (filedes == -1){

        printf(" ERROR: File not found. \n");
        return -1;

    } 
    else{

        int startingBlock = dir1->entry[filedes].beginningIndex;
        while (fat->file[startingBlock].nextBlock != -1){

            startingBlock = fat->file[startingBlock].nextBlock;
        }

        int bufferLength = strlen((char*)buf);
        while (bufferLength > 0){
            printf(" Starting to write at block: %d\n", startingBlock);
            printf(" Buffer Length: %d\n", bufferLength);
            
            int fileOffset = findFileOffset(dir1, filedes);
            printf(" Offset: %d\n", fileOffset);

            int blockNumber;
            if(bufferLength < (BLOCK_SIZE - fileOffset)){
                blockNumber = bufferLength;
            }
            else{
                blockNumber = BLOCK_SIZE - fileOffset;
            }
            printf(" Block Number: %d\n", blockNumber);

            strncpy(Data->blocks[startingBlock].sect + fileOffset, (char *)buf + (strlen((char*)buf) - bufferLength), blockNumber);
            
            updateValue(&dir1->entry[filedes].fileSize, blockNumber + dir1->entry[filedes].fileSize);
            char *strBuf = (char *)malloc(sizeof(nbyte+1));
            strcpy(strBuf, (char *)buf);
            
            
            printf(" Filesize: %d\n", dir1->entry[filedes].fileSize);
            
            bufferLength = bufferLength - blockNumber;
            
            int freeBlock = findAvailableBlock(fat);

            block_write(freeBlock, strBuf);
            if ((fileOffset + blockNumber) == BLOCK_SIZE){

                
                fat->file[startingBlock].nextBlock = freeBlock;
                fat->file[freeBlock].status = BUSY;
                fat->file[freeBlock].nextBlock = -1;
                startingBlock = freeBlock;
            }
        }
    }
    memcpy(&(Data->blocks[peek()]), dir1, sizeof(*dir1));
    return 0;
}


//Helper function to write to the file
void writeForFile(char *fileName){
    
    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));
    int filedes = findFileEntry(fileName, dir1);
    
    char buf[1024] = "";
    printf("\nWrite to a file: ");
    scanf("%s", buf);
    size_t len = strlen(buf);
    fs_write(filedes, buf, len);
}

/*Set up the directory as usual then 
* 
* 
* 
*/
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

        int startBlock = dir1->entry[filedes].beginningIndex;
        


        while(fat->file[startBlock].nextBlock != -1){
            memset(tempBuf, '\0', sizeof(tempBuf));
            memcpy(tempBuf, Data->blocks[startBlock].sect, BLOCK_SIZE);
            strcat(buf, tempBuf);
            startBlock = fat->file[startBlock].nextBlock;
        }

        int offset = findFileOffset(dir1, filedes);
        memset(tempBuf, '\0', sizeof(tempBuf));
        memcpy(tempBuf, Data->blocks[startBlock].sect, offset);
        strcat(buf, tempBuf);

        block_read(startBlock, tempBuf);
        printf("%s\n", buf);
    }
    memcpy(&(Data->blocks[peek()]), dir1, sizeof(*dir1));
    return 0;
}

void readFromFile(char *fileName){
    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));
    int filedes = findFileEntry(fileName, dir1);
    
    char buf[1024] = "";
    
    size_t len = strlen(buf);
    fs_read(filedes, (void *)buf, len);
}


// WRITE DATA TO FILE

//return top of the directory stack
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
        directoryStack[topOfStack] = directoryBlock;
    }
}


//Print the whole directory stack
void printStack(){
    puts("\nDIRECTORY STACK: \n");
    int loop;
    int numEntries;
    //getting the size of the array or stack
    int size = sizeof(directoryStack)/sizeof(int);
    for(loop = 0; loop < size; loop++){
        printf("%d ", directoryStack[loop]);
    }
    numEntries = getNumEntries();
    printf("\n\nNumber of entries: %d\n\n", numEntries);
}

//get the number of entries that are currently in the stack
int getNumEntries(){
    int count = 0;
    int i = 0;
    int size = sizeof(directoryStack)/sizeof(int);
    for(i = 0; i < size; i++){
        if(directoryStack[i] > 0){
            count++;
        }
    }
    //becuase root directory is in beginning as a zero
    count+=1; 
    return count;
}

//Swith between directories in the file system just like a regular linux terminal.
//In order to do so in the file system, the directory stack helps changing to a 
//new directory by pushing an element's beginningIndex from the directory entry table. 
int changeDirectory(char *directoryName){
    //If the user types "cd .." the function will then pop off the current directory from the 
    //directory stack and then point to the next directory on the stack which is the previous directory
    if (strcmp(directoryName, "..") == 0){
        pop();
        return 1;
    }
    //initialize space for a directory value with size of the value
    directory *dir1 = (directory *)malloc(sizeof(*dir1));
    memcpy(dir1, &(Data->blocks[peek()]), sizeof(*dir1));


    for (int i = 0; i < MAX_ENTRIES; i++){
        if ((strcmp(directoryName, dir1->entry[i].fileName) == 0) && (dir1->entry[i].type == DIR_TYPE)){
            push(dir1->entry[i].beginningIndex);
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

char **pathPaser (char *input){ 
    int bufferSize = 1024, index = 0; //for the string token position in tokens [index] 
    char **tokens = (char **)malloc(bufferSize * sizeof(char *)); 
    // allocating space for the tokens and its double pointers because elements in the array are  
    // pointers, the array must be a pointer to a pointer 
    char *token; // represents a token in the array of tokens  
    token = strtok(input, " /"); // This  -->  " \t\n" represents the whitespaces that are common 
    while(token != NULL) {  
        tokens [index] = token; 
        // Maybe check ifthere needs to be a reallocation ifthe bufferSize somehow is less than index. 
        token = strtok(NULL, " /"); // place null terminated \0 at the end of each token 
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
    else if(strcmp(parsedArgs[0], "unmount_fs") == 0){
        unmount_fs(parsedArgs[0]);
    }
    else if(strcmp(parsedArgs[0], "printStr") == 0){
        printStr(tempStr);
    }
    /*else if(strcmp(parsedArgs[0], "mount_fs") == 0){
        mount_fs(parsedArgs[1]);
    }*/
    else if(strcmp(parsedArgs[0], "fs_open") == 0){
        fileDescriptor = fs_open(parsedArgs[1]);
    }
    else if(strcmp(parsedArgs[0],"fs_close") == 0){
        fs_close(fileDescriptor);
    }
    else if(strcmp(parsedArgs[0], "getSize")){
        fs_get_fileSize(fileDescriptor);
    }
    
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
    char diskName[15] = "TESTDRIVE";
    
    puts("\n------MY FILE SYSTEM------\n");

    directoryStack[topOfStack] = 0;

    make_fs(diskName);

    char input[1024];
    shellPrompt(input);
    int count = 0;
    while(1){
        char **args = shellParser(input);
        builtInCommands(args);
        free(args);
        shellPrompt(input);
    }
   
    return 0;
}