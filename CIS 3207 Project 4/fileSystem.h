#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "disk.h"

#define NUM_BLOCKS 8192

typedef enum{
    file, directory
}fileType;
typedef enum{ 
    False, True 
}boolean;

typedef struct{
    char name [15];	// name of file or directory
    int size;		// size of file in bytes, it is 0 for directory
    fileType typeFlag: 1;		// a flag that indicates whether data entry is a file or directory 
    int firstBlock: 1;	// first block of the file/directory, points to a block that contains file or //subdirectory
    int numBlocks;
    int fdCount;
    int first_block;
    boolean empty; //whether the file/directory is being used
}directoryEntry;



typedef struct{
    // Directory info
    int dirIndex;
    int dirLen;
    // Data info
    int dataIndex;
}superBlock;


// file information
typedef struct{
    boolean empty;               // whether the file is empty or being used
    char name[15];               // file name
    int size;                    // file size
    int head;                    // first data block
    int numBlocks;               // number of blocks
    int fdCount;                 // number of file descriptors using this file
} fileInfo;

//file descriptor
typedef struct{
    boolean empty;               // whether the file descriptor is empty or being used
    char file;                   // file index
    int offset;                  // read offset used by fs_read()
}fileDescriptor;


typedef struct{
    char file[BLOCK_SIZE];
    directoryEntry directory[BLOCK_SIZE];
}block;

typedef struct{
    directoryEntry *metadata;	 // represents the file’s metadata 
    int currentBlock;		     // current block that the file is in
    int pointer;			     // pointer to the current file’s location
}filePointer;


typedef struct {
    int FAT[NUM_BLOCKS];
    block data[NUM_BLOCKS]; 
} drive;


int make_fs(char *);
int mount_fs(char *);
int unmount_fs(char *);
int fs_open(char *);
int fs_close(int);
int fs_create(char *);
int fs_delete(char *);
int fs_mkdir(char *);
int fs_read(int, void *, size_t);
int fs_write(int, void *, size_t);
int fs_get_filesize(int);
int fs_lseek(int, off_t);
int fs_truncate(int, off_t);
#endif