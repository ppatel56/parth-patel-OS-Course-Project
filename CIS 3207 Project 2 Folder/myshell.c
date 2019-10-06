#include <stdio.h> 
#include <stdlib.h> 
//#include <io.h.> 
#include <string.h>  
#include <dirent.h> 
#include <sys/types.h>
#include <unistd.h>

void shellPrompt();
char **parser (char *input);  
//parses out the string input by get the tokens and eliminate the white space 
int piping(char *input); 
//This will take in the string input and parse it using the parsing function, then create two parameters  
// “parameter 1 | parameter 2”, since we are dealing with strings, the parameters are char[num]    
void currentDirectory (); //This function acts like “ls” command in Linux 
void clear (); 
//NOTE to clear screen I can print (“033[H 033[2J”) since system () cannot be used 
void changeDirectory (char **args); //chdir(newDir) will change current working directory to newDir 
void pausing();  
void quit (); 
void echo (char **args); 
void help (); 
void environment (); 
int getCommandInput (char *input, char **parsedArgs);  
// Check ifthe commands are built in the shell or not such as clear () or “directory” 
int backgroundExecution (char *input);  
//checks ifthe commands are external or built-in using getCommandInput(char *input); and does //external execution ifthe input is not found in built-in commands, also ifredirection or piping happens 
 
int redirection (char *input); 
// sends the output to a file rather than screen  
 


void shellPrompt(char *input){
    char cwd[1024]; //current working directory
    getcwd(cwd, sizeof(cwd));
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        printf("%s$ ",cwd);
        fgets(input, 1024, stdin);
    }
} 
 
/* 
 The parser function is to pass in the input from the user and makes it into constituent pieces of executable code. 
The critical part of the function is the strtok () function which helps split the input commands from user. 
In this function the strtok () is used to delimit the white spaces out of the input.  
*/  
char **parser (char *input) { 
    int bufferSize = 1024, index = 0; //for the string token position in tokens [index] 
    char **tokens = malloc (bufferSize * sizeof(char *)); 
    // allocating space for the tokens and its double pointers because elements in the array are  
    // pointers, the array must be a pointer to a pointer  
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
 
 
/*  
Basically, the function prints out the string given which will clear the outputs of the terminal 
This will be used in getCommandInput when if-else statements are used for the built-in commands 
It doesn’t need any inputs since it just uses the printf function. 
*/ 
void clear() { 
printf ("033[H 033[2J"); 
} 
/* 
This function calls the exit function which will end the program.  
And just like clear (), this function doesn’t need any inputs passed in. 
*/ 
void quit() { 
    puts("Exiting the Program"); 
exit(0); 
}
/* 
The pause () basically pauses the terminal until the user hits the “Enter” key which is “\n” 
ifthe user types in any other character, the program will still be paused until the user hits the “Enter” key 
The getchar () is used because it takes in any character that is inputted 
*/ 
void pausing() { 
    char character; // 
    puts("Shell program is paused until ENTER or RETURN are pressed."); 
    // Using getchar () in the whileloop argument, the user can type any character on the one line, 
    //until ENTER is pressed which will exit out of the pause () and go back to normal. 
    while((character = getchar ()) != '\n'){
        // The whileloop is empty because it is endless until ENTER is pressed
    }  
} 
/* 
Gets the built-in command list and information on piping, redirection, and external page from README.txt 
Using fgetc () function to get every character in the file without any trouble whilefscanf () could potentially read in the file incorrectly 
*/ 
void help () { 
    FILE *helpFile; 
    char character; 
    helpFile = fopen("README.txt", "r"); 
    if(helpFile == NULL) { 
    puts("File not found"); 
    } 
    else { 
        //Loop till the character has not reached End of File 
        while(character != EOF) { 
        character = fgetc (helpFile); 
        printf("%c", character);  
        } 
    } 
fclose(helpFile); 
} 
 
 
/* 
The function basically gets all the information currently in the path such as USER, PATH, NAME, SHELL, etc. 
Easiest way of doing this is to use the existing function of getenv () which will get the environment string of the constant string such as USER i.e. getenv (“USER”) will retrieve whatever is in USER. 
Assign getenv() to a string pointer then print the string. 
*/ 
void environment () { 
    char *string; 
    string = (char *)malloc(2000*sizeof(char)); 
    string = getenv("HOSTTYPE"); 
    printf("\nHOSTTYPE=%s", string);
    string = getenv("LESSCLOSE"); 
    printf("\nLESSCLOSE =%s", string);
    string = getenv("LANG"); 
    printf("\nLANG =%s", string);  
    string = getenv("WSL_DISTRO_NAME"); 
    printf("\nWSL_DISTRO_NAME =%s", string); 
    string = getenv("USER"); 
    printf("\nUSER=%s",string);
    string = getenv("PWD"); 
    printf("\nPWD =%s", string);
    string = getenv("HOME"); 
    printf("\nHOME =%s", string);
    string = getenv("NAME"); 
    printf("\nNAME =%s", string);
    string = getenv("XDG_DATA_DIRS"); 
    printf("\nXDG_DATA_DIRS =%s", string); 
    string = getenv("SHELL");
    printf("\nSHELL=%s",string);
    string = getenv("TERM"); 
    printf("\nTERM =%s", string);
    string = getenv("SHLVL"); 
    printf("\nSHLVL =%s", string);
    string = getenv("LOGNAME"); 
    printf("\nLOGNAME =%s", string);
    string = getenv("PATH"); 
    printf("\nPATH =%s", string);
    string = getenv("WSLENV"); 
    printf("\nWSLENV =%s", string);
    string = getenv("LESSOPEN"); 
    printf("\nLESSOPEN =%s\n", string);
    
    
/* 
This will repeat till all the environment variables are in the function. They will be in the actual program/code. 
*/ 
 
} 
/* 
The function echo () is simple as it will print out the statement after the “echo” i.e. input: echo hello there and the output: hello there 
Since the input is parsed, the args[0] is already echo, so whatever statement after is args[1] args[2]… 
Echo hello there --> args[0] args[1] args[2] 
*/ 
void echo (char **args) { 
    int index = 1; 
    while(args[index] != '\0'){ 
        printf("%s ", args[index]); 
        index++; 
    }
    printf("\n");  
} 
/* 
This function is the equivalent to “ls” which lists the contents (i.e. files) in the current directory. 
There is a struct dirent from the dirent.h which constructs directory traversal  
*/ 
void currentDirectory (char **args) { 
    DIR *dir; 
    struct dirent *dirEntry;  
    char *directory; //Name of the directory  
    dir = opendir (directory); // opendir () checks ifdirectory has NULL or not 
    // while there is another item in the directory not looked at yet 
    while((dirEntry = readdir (dir)) != NULL) { 
        printf("%s\t", dirEntry->d_name); //Print out all the contents 
    } 
    // The function getcwd() could also be beneficial in getting the current directory  
} 
 
/* 
Change directory is just “cd” 
There will be a function called chdir () used that will pass in the new directory variable.  
This will be vaguely similar to the echo () function where args[1] will be the new directory.  
*/ 
void changeDirectory (char **args) { 
    char *newDir = args[1]; 
//    if(chdir(newDir) == 0){ 
//nothing is put here because you don’t need to print anything out 
//    }

    //if chdir does not return zero then error
    if(chdir(newDir) != 0){ 
        printf("No such directory found\n"); 
    } 
} 
 
 
 
 
/* 
Strcmp () will be used because the user input commands will be compared to the built-in command string i.e strcmp (input[0], “clr”); it is input[0] because it is the first command 
The getCommandInputwill take in the argument from the user and determines what the string input is with if-else statements in the function i.e. if(strcmp (input[0], “clr”)) { then use clear(input)} which will clear the screen.  
The type of the function is int because it is going to return a number whether it be 0 or -1, so that this function will be used more in the more complex functions such as redirection or piping 
NOTE: The inputs will be parsed before passed in the function every time.  
*/ 
int getCommandInput(char *input, char **parsedArgs) { 
    if(parsedArgs[0] == NULL){  
        // if the user doesn’t input a command that are built-in or external i.e. fdfd or \n 
        puts("Command doesn’t exit. Try again."); 
        return 0;   
    } 
    if(strcmp (parsedArgs[0], "quit")==0){ 
        puts("Exiting the Program"); 
        exit(0); 
    } 
    else if(strcmp(parsedArgs[0], "clr")==0) { 
        clear(); 
        return 0; 
    }
    
    else if(strcmp (parsedArgs[0], "pause")==0) { 
        pausing();
        return 0;
    }
    if(strcmp (parsedArgs[0], "help")==0) { 
        help();
        return 0;
    } 
    else if(strcmp (parsedArgs[0], "echo")==0) { 
        echo(parsedArgs);
        return 0;
    } 
    else if(strcmp (parsedArgs[0], "dir")==0){ 
        currentDirectory(parsedArgs); 
        return 0; 
    } 
    else if(strcmp (parsedArgs[0], "cd")==0){ 
        changeDirectory(parsedArgs); 
        return 0; 
    } 
    else if(strcmp (parsedArgs[0], "environ")==0) { 
        environment(); 
        return 0; 
    } 
    // ifthe function fails, return -1 
    return -1; 
    } 
 
/* 
Redirection is to do input/and or output to or from a file. 
Using strcmp to check ifthere is an element in the array that has the redirection symbols “<”,”>”,”>>” 
Functions such as fork() for creating a child process, and dup2() which creates new file descriptor, and execvp () launches the external command   
*/ 
int redirection (char *input) { 
    //first parse the input line 
    char **args = parser (input); 
    int *inputFile; //int is used because of the dup2() passes in inputs 
    int *outputFile; //int is used because of the dup2() passes in inputs   
    int index = 0; 
 
    /* 
    Before the loop try to create new child process with fork() and check ifit was successful then continue on with the loop    
    */ 

    while(args[index] != NULL) { 
        if(strcmp (args[index], "<")) { 
            //Do multiple steps in reading from input to a file and ifthe file doesn’t then print out that the file doesn’t exit. 
            return 0; // If"the part was successf   l"   
        }    
        else if(strcmp(args[index], ">")){ 
            // “>” means to truncate or overwrite the file 
            //Do multiple steps in writing out to an output file and overwriting the contents in the existing file or iffile does not exist then create a new one which will involve even more steps with an if-statement to check ifthe files exits. 
            return 0; // If"the part was successf   l"   
        } 
        else if(strcmp(args[index], ">>")) { 
            // “>>” means to append to the file 
            //Do multiple steps in writing out to an output file and append contents in the existing file or iffile does not exist then create a new one which will involve even more steps with an if-statement to check ifthe files exits. 
            return 0; // ifthe part was successful 
        } 
        index += 1;
    } 
    return -1; //iffunction fails, return -1 
} 
 
 
 
 
/* 
The function for pipe () parameter [0] will be the file descriptor for the read end of pipe. Parameter [1] will be the file descriptor for the write end of pipe 
Just like redirection, the function will be an int so it will return 0 on success or return -1 on failure. 
The functions fork(), wait() which is process waiting for the other process to finish, and execvp() might be used 
whileloop is used to find the “|” pipe symbol 
*/ 
int piping(char *input){ 
//First pares the input. 
    char **args = parser(input); 
    int index = 0; 
    char *first[30], *second[30], *pipeSym[1]; // assign the first and second parameters when finding the pipe symbol 
    /*while(args[index] != "|") { 
        
        
        first[index] = args[index];

        index++; 
    }
    pipeSym[index] = "|";
    index++;
    while(pipeSym[index-1] == "|" && args[index] != NULL){

    }*/
    while(args[index] != NULL){

        first[index] = args[index];

        if(strcmp (args[index], "|")){ 
        // Try to create the two new child processes using fork() and using dup2(), and execvp(second)   
        //break at the end of the if-statement
        //first[index-1] = args[index];
        //second = args[index+1];
        break; 
        }
        index++;
    }
    index++; //increment the index to the next element in the array after "|" or AKA the second parameter
    while(args[index] != NULL){
        second[index] = args[index];
        index++;
    }

    

return 0; // return 0 ifsuccessful 
} 




/*
int pipe(char *input){
    char **args = parser(input);

    return 0;
}*/



int backgroundExecution(char *input){
    //First pares the input. 
    char **args = parser(input); 
    int index = 0; 
    char *first[30], *second[30];

    while(args[index] != NULL){

    }

    return 0;
}





















int main(){
    char input[1024];
    
    shellPrompt(input);
    
   /* 
    //TEST FOR PARSING A STRING
    puts("Hello");
    char **parse = parser(input);
    puts("There");
    int i = 0;
    while(parse[i] != '\0'){
        printf("Token %d : %s\n", i,parse[i]);
        i++;
    }
    i++;
    puts("Bye");*/
    char **args = parser(input);
    //getCommandInput(input, args);
    while(1){
    
        getCommandInput(input, args);


        shellPrompt(input);
    }


    
    return 0;
}