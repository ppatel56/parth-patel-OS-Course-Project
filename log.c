#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "setup.h"
#include "log.h"

int main(){
    inputFile = fopen("input.txt", "r");
    SEED = getInputValuesInt();
    INITIAL_TIME = getInputValuesInt();
    FINAL_TIME = getInputValuesInt();
    ARRIVE_MIN = getInputValuesInt();
    ARRIVE_MAX = getInputValuesInt();
    QUIT_PROB = getInputValuesDouble();
    CPU_MIN = getInputValuesInt();
    CPU_MAX = getInputValuesInt();
    DISK1_MIN = getInputValuesInt();
    DISK1_MAX = getInputValuesInt();
    DISK2_MIN = getInputValuesInt();
    DISK2_MAX = getInputValuesInt();
    fclose(inputFile);
    printf("INPUT TIME: %d\nFINAL TIME: %d\nARRIVE MIN: %d\nARRIVE MAX %d\n",INITIAL_TIME,FINAL_TIME,ARRIVE_MIN,ARRIVE_MAX);
    printf("QUIT PROB: %.1f\nCPU MIN: %d\nCPU MAX: %d\nDISK1 MIN: %d\nDISK1 MAX: %d\n",QUIT_PROB,CPU_MIN,CPU_MAX,DISK1_MIN,DISK1_MAX);
    printf("DISK2 MIN: %d\nDISK2 MAX: %d\n",DISK2_MIN,DISK2_MAX);
    
    return 0;
}

/*
Get values from the input files by converting strings into ints and doubles
using atoi and atof functions
*/

int getInputValuesInt(){
    int value;
    char strVariable[255];
    fscanf(inputFile,"%s",strVariable);
    fscanf(inputFile, "%d",&value);
    return value;
}

double getInputValuesDouble(){
    double value;
    char strVariable[255];
    fscanf(inputFile,"%s",strVariable);
    fscanf(inputFile, "%lf", &value);
    return value;
}

/*
*Log file stuff maybe put the logs in the STAT file as well.
*Might have to put fprintf() during the simulation event handling function for the logs e.g. "Process number 100 entered CPU at time 120".
*STAT file maight have its own functions in this log.c file 
*/