#include <stdio.h>
#include <stdlib.h>
#include "disk.h"
#include "fileSystem.h"

void createFile (virtualDisk *vDisk, int type, char *path);
filePointer *openFile (virtualDisk *vDisk, char *path);
void closeFile (filePointer *file);
void readFile (virtualDisk *vDisk, filePointer *file);    
void writeFile (filePointer *file, virtualDisk *vDisk, char *data);
void deleteFile (virtualDisk *vDisk, char *fileName);

int main(){
    return 0;
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

void writeFile (filePointer *file, virtualDisk *vDisk, char *data){

}

void deleteFile (virtualDisk *vDisk, char *fileName){
    
}
