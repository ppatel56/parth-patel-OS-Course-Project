#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "disk.h"
#include "fileSystem.h"

void createFile (virtualDisk *vDisk, int type, char *path);
filePointer *openFile (virtualDisk *vDisk, char *path);
void closeFile (filePointer *file);
void readFile (virtualDisk *vDisk, filePointer *file);    
void writeFile(virtualDisk *vDisk, filePointer *file, char *path);
void deleteFile (virtualDisk *vDisk, char *fileName);
int allocateBlocks(virtualDisk *vDisk);
dataEntry *getMetaData(virtualDisk *vDisk, char *path);
//void *getMeta(char *path);
int make_fs(char *diskName);
int mount_fs(char *diskName);
int unmount_fs(char *diskName);

int main(){
    //puts("Hello World");
    char *string = "Hello World";
    getMeta(string);
    return 0;
}

int allocateBlocks(virtualDisk *vDisk){
    int i;
    for(i=0; i < NUM_BLOCKS; i++){
        //if a block is empty
        if(vDisk->FAT[i] == 0){
            //set the particular block to occupied
            vDisk->FAT[i] = -1;
            return i;
        }
    }
    //if all the blocks are full
    return -1;
}

dataEntry *getMetaData(virtualDisk *vDisk, char *path){
    char *pathCopy = (char *)malloc(sizeof(char *) *strlen(path)+1);
    strcpy(pathCopy, path);
    dataEntry *metaData = (dataEntry *)malloc(sizeof(dataEntry *));
    //If there is no path
    if(strcmp(pathCopy, "")==0){
        dataEntry *rootDir = (dataEntry *)malloc(sizeof(dataEntry *));
        rootDir->typeFlag = directory;
        rootDir->firstBlock = 0;
        return rootDir;
    }
    
}





void createFile (virtualDisk *vDisk, int type, char *path){

}

filePointer *openFile (virtualDisk *vDisk, char *path){
    filePointer *openedFile;
    
    
    return openedFile;
}

void closeFile(filePointer *file){

    free(file);
}

void readFile (virtualDisk *vDisk, filePointer *file){

}

void writeFile(virtualDisk *vDisk, filePointer *file, char *path){

}

void deleteFile (virtualDisk *vDisk, char *fileName){
    
}

int make_fs(char *diskName){
    int diskCreated = make_disk(diskName);
    if(diskCreated == 0){
        //Initialize the structures here
    } 
    return 0;
}

int mount_fs(char *diskName){
    //basically open the disk to be written
    //maybe try to initialize/hard code the Super Block (home/root directory)
    return 0;
}

int unmount_fs(char *diskName){
    //first close all of the files and then close the disk
    //make sure that its persistent?
    return 0;
}