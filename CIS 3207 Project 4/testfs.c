#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "disk.h"
#include "fileSystem.h"
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