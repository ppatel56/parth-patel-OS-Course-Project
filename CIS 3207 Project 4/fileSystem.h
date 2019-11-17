#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_
#include "disk.h"

#define NUM_BLOCKS 8192
/******************************************************************************/
typedef struct{
    char name [30];	// name of file or directory
    int size;		// size of file in bytes, it is 0 for directory
    int typeFlag;		// a flag that indicates whether data entry is a file or directory 
    int firstBlock;	// first block of the file/directory, points to a block that contains file or //subdirectory
}dataEntry;



typedef struct{
    char file[BLOCK_SIZE];
    dataEntry directory[BLOCK_SIZE];
}block;

typedef struct{
    dataEntry *metadata;	 // represents the file’s metadata 
    int currentBlock;		// current block that the file is in
    int pointer;			// pointer to the current file’s location
}filePointer;


typedef struct {
    int FAT[NUM_BLOCKS];
    block data[NUM_BLOCKS]; 
} virtualDisk;

void createFile (virtualDisk *vDisk, int type, char *path);
filePointer *openFile (virtualDisk *vDisk, char *path);
void closeFile (filePointer *file);
void readFile (virtualDisk *vDisk, filePointer *file);    
void writeFile (filePointer *file, virtualDisk *vDisk, char *data);
void deleteFile (virtualDisk *vDisk, char *fileName);


#endif