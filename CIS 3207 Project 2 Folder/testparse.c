#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>  
#include <dirent.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char **parser (char *input);  
//parses out the string input by get the tokens and eliminate the white space 

 
/* 
 The parser function is to pass in the input from the user and makes it into constituent pieces of executable code. 
The critical part of the function is the strtok () function which helps split the input commands from user. 
In this function the strtok () is used to delimit the white spaces out of the input.  
*/  
char **parser (char *input) { 
    int bufferSize = 1024, index = 0; //for the string token position in tokens [index] 
    char **tokens = malloc (bufferSize * sizeof(char *)); 
    // pointers, the array must be a pointer to a pointer  
    // allocating space for the tokens and its double pointers because elements in the array are  
    char *token; // represents a token in the array of tokens  
    token = strtok (input, " \t\n"); // This  -->  " \t\n" represents the whitespaces that are common 
    while(token != NULL) {  
        tokens [index] = token; 
        // Maybe check ifthere needs to be a reallocation ifthe bufferSize somehow is less than index. 
        token = strtok (NULL, " \t\n"); // place null terminated \0 at the end of each token 
        index++; 
    } 
    tokens[index] = NULL;
    return tokens; 
    } 
 
int main(){
    char input[1024] = "Hello.txt | World";
    
    
    
    //TEST FOR PARSING A STRING
    //puts("Hello");
    char **parse = parser(input);
    //puts("There");
    int i = 0;
    while(parse[i] != '\0'){
        printf("Token %d : %s\n", i,parse[i]);
        i++;
    }
    i++;
    //puts("Bye");



    return 0;
}