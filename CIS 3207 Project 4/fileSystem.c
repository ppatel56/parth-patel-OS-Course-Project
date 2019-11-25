#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "disk.h"
#include "fileSystem.h"

#define SUPER_BLOCK 0
#define MAX_NUM_FD 32
#define MAX_FILE_SIZE 64


int make_fs(char *diskName);
int mount_fs(char *diskName);
int unmount_fs(char *diskName);
int fs_open(char *name);
int fs_close(int fd);
int fs_create(char *name);
int fs_delete(char *name);
int fs_mkdir(char *name);
int fs_read(int fd, void *buf, size_t nbyte);
int fs_write(int fd, void *buf, size_t nbyte);
int fs_get_filesize(int fd);
int fs_lseek(int fd, off_t offset);
int fs_truncate(int fd, off_t length);

directoryEntry NULL_ENTRY = {0};
directoryEntry *rootDir;
superBlock *superBlockPtr;
fileDescriptor fdTable[MAX_NUM_FD];
directoryEntry *dirEntry;

int main(){
    //puts("Hello World");
    char *diskName = "DRIVE";
    if(make_fs(diskName) < 0) {
        fprintf(stderr, "make_fs()\t error.\n");
    }

    if(mount_fs(diskName) < 0) {
        fprintf(stderr, "mount_fs()\t error.\n");
    }
    if(unmount_fs(diskName) < 0) {
        fprintf(stderr, "unmount_fs()\t error.\n");
    }

    if(mount_fs(diskName) < 0) {
        fprintf(stderr, "mount_fs()\t error.\n");
    }
    return 0;
}

/*
int make_fs(char *diskName){
    int diskCreated = make_disk(diskName);
    if(diskCreated == 0){

        int diskOpen = open_disk(diskName);
        if(diskOpen==0){
            //Initialize the structures here    
            virtualDisk *disk = (virtualDisk *)malloc(sizeof(virtualDisk *));
            char *buffer = malloc(sizeof(BLOCK_SIZE));

            
            dataEntry *rootDir = (dataEntry *)malloc(sizeof(dataEntry *));
            rootDir->typeFlag = directory;
            rootDir->firstBlock = SUPER_BLOCK;
            
            //Below  is redundant code
            int supBlock = rootDir->firstBlock;
            supBlock = disk->FAT[supBlock];
            //Above is redundant code
            
            printf("\n\nRoot Directory: \n\tBlock number in FAT = %d\n", disk->FAT[0]);
            printf("\trootDir->firstBlock = %d\n\n", rootDir->firstBlock);
            free(buffer);
        }
        else{
            puts("Could not open the disk");
            return -1;
        }
    }
    else{
        puts("Could not create the disk");
        return -1;
    }
    
    return 0;
}
*/

int make_fs(char *diskName){
    if(make_disk(diskName) != 0 || open_disk(diskName) != 0){
        return -1;
    }
    //intialize the super block 
    superBlockPtr = (superBlock *)malloc(sizeof(superBlock));
    //initialize the root directory
    //rootDir = (directoryEntry *)malloc(sizeof(directoryEntry *));
    if(superBlockPtr == NULL){
        return -1;
    }
    //if(rootDir == NULL){
    //    return -1;
    //}
    superBlockPtr->dirIndex = 1;
    superBlockPtr->dirLen = 0;
    superBlockPtr->dataIndex = 2;

    //rootDir->typeFlag = directory;
    //rootDir->firstBlock = SUPER_BLOCK;
    
    //create buffer so that it can be used to help the super block 
    //be written to the disk at block 0
    char buffer[BLOCK_SIZE] = "";

    //memset() function writes len bytes of value c (converted to an unsigned char)
    //to the string b.
    memset(buffer, 0, BLOCK_SIZE);

    //memcpy() function copies n bytes from memory area source to memory area destination.
    memcpy(buffer, &superBlockPtr, sizeof(superBlock));

    block_write(0, buffer);
    close_disk();
    printf("Successfully created file system [%s]\n", diskName);
    return 0;
}

int mount_fs(char *diskName){
    //Basically open the disk to be written
    //Maybe try to initialize/hard code the Super Block (home/root directory)

    //check if diskName is valid OR check if disk can open with diskName, otherwise return -1
    if(diskName == NULL || open_disk(diskName) != 0){
        return -1;
    }
    
    //read the super block
    char buffer[BLOCK_SIZE] = "";
    memset(buffer, 0, BLOCK_SIZE);
    block_read(0, buffer);
    memcpy(&superBlockPtr, buffer, sizeof(superBlockPtr));

    //read directory entry information
    dirEntry = (directoryEntry*)malloc(BLOCK_SIZE);
    memset(buffer, 0, BLOCK_SIZE);
    block_read(superBlockPtr->dirIndex, buffer);
    memcpy(dirEntry, buffer, sizeof(directoryEntry) * superBlockPtr->dirLen);

    // clear file descriptors 
    int i = 0;
    for(i = 0; i < MAX_NUM_FD; ++i) {
        fdTable[i].empty = True;
    }
    printf("Successfully mounted file system [%s]\n", diskName);
    return 0;
}

int unmount_fs(char *diskName){
    //First close all of the files and then close the disk
    //Make sure that its persistent?
    //So that if the you open the file system again, the files/directories
    //will still be there
    
    if(diskName == NULL) return -1;

    // write directory info */
    
    directoryEntry* filePtr = (directoryEntry*)dirEntry;
    char buffer[BLOCK_SIZE] = "";
    memset(buffer, 0, BLOCK_SIZE);
    char* blockPtr = buffer;


    int i = 0;
    for (i = 0; i < MAX_FILE_SIZE; i++){
        if(dirEntry[i].empty == True){
            memcpy(blockPtr, &dirEntry[i], sizeof(dirEntry[i]));
            blockPtr = blockPtr + sizeof(directoryEntry);
        }
    }

    block_write(superBlockPtr->dirIndex, buffer);
    int j = 0;
    // clear file descriptors
    for(j = 0; j < MAX_NUM_FD; j++){
        if(fdTable[j].empty == 1) {
            fdTable[j].empty = False;
            fdTable[j].file = -1;
            fdTable[j].offset = 0;
        }
    }

    free(dirEntry);
    close_disk();
    printf("successfully: file system [%s] umounted.\n", diskName);
    
    return 0;
}