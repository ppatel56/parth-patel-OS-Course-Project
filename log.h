#ifndef LOG
#define LOG

#include <stdio.h>
/*Implementing input file, config file, and stat file.*/

int getInputValuesInt();
double getInputValuesDouble();

FILE *logFile;
FILE *inputFile;
FILE *statFile;
#endif // INPUT