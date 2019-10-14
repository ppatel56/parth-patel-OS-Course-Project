#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>  
#include <dirent.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void shellPrompt();
//parses out the string input by get the tokens and eliminate the white space
char **parser(char *input); 


//This will take in the parsed input then create two parameters  
// “parameter 1 | parameter 2”, since we are dealing with strings, the parameters are char[num]
int piping(char **input); 


//This function acts like “ls” command in Linux 
void currentDirectory(); 

//NOTE to clear screen I can print (“033[H 033[2J”) since system () cannot be used 
void clear();
// returns 1 to break out of main while loop 
int quit(); 
void changeDirectory(char **args); //chdir(newDir) will change current working directory to newDir 
void pausing();  
void echo(char **args); 
void help(); 
void environment(); 

// Check ifthe commands are built in the shell or not such as clear () or “directory”
int builtInCommands (char **parsedArgs);  


//checks ifthe commands are external or built-in using builtInCommands(char *input); and does 
//external execution ifthe input is not found in built-in commands, also ifredirection or piping happens
int normalExecution(char **input);

//Does background execution
int backgroundExecution(char **input);

//Checks for piping, I/O redirection, background, built-in commands, and external commands. Then execute the functions  
int checkAndExecute(char **args);

void getBatchfile(char *fileName);


int redirection(char **input);


/*
The shellPrompt basically creates a string array that gets the current working directory and prints it out as myshell is running
The shellPrompt takes in an input and passes in the fgets() 
*/
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
    char **tokens = malloc(bufferSize * sizeof(char *)); 
    // allocating space for the tokens and its double pointers because elements in the array are  
    // pointers, the array must be a pointer to a pointer 
    char *token; // represents a token in the array of tokens  
    token = strtok(input, " \t\n"); // This  -->  " \t\n" represents the whitespaces that are common 
    while(token != NULL) {  
        tokens [index] = token; 
        // Maybe check ifthere needs to be a reallocation ifthe bufferSize somehow is less than index. 
        token = strtok(NULL, " \t\n"); // place null terminated \0 at the end of each token 
        index++; 
    } 
    tokens[index] = NULL;
    return tokens; 
} 
 
 
/*  
Basically, the function prints out the string given which will clear the outputs of the terminal 
This will be used in builtInCommands when if-else statements are used for the built-in commands 
It doesn’t need any inputs since it just uses the printf function. 
*/ 
void clear(){
    printf ("%s","\e[1;1H\e[2J");
} 
/* 
This function calls the exit function which will end the program.  
And just like clear (), this function doesn’t need any inputs passed in. 
*/ 

int quit(){
    //puts("Exiting my shell");
    return 1;
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
void help(){ 
    FILE *helpFile; 
    char character; 
    helpFile = fopen("README", "r"); 
    if(helpFile == NULL) { 
    puts("File not found"); 
    } 
    else { 
        //Loop till the character has not reached End of File 
        while(character != EOF) { 
            character = fgetc(helpFile); 
            //printf("%c", character);
            putchar((int)character);  
        }
        printf("%s","\n"); 
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
    string= getenv("LS_COLORS");
    printf("\nLS_COLORS=%s", string); 
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
 
} 

/* 
The function echo () is simple as it will print out the statement after the “echo” i.e. input: echo hello there and the output: hello there 
Since the input is parsed, the args[0] is already echo, so whatever statement after is args[1] args[2]… 
Echo hello there --> args[0] args[1] args[2] 
*/ 
void echo(char **args) { 
    int index = 1; 
    while(args[index] != NULL){ 
        printf("%s ", args[index]); 
        index++; 
    }
    printf("\n");  
}  
/* 
This function is the equivalent to “ls” which lists the contents (i.e. files) in the current directory. 
There is a struct dirent from the dirent.h which constructs directory traversal  
*/ 
void currentDirectory() {
    DIR *dir; 
    struct dirent *dirEntry;
    //char *directory = (char *)malloc(1024*sizeof(char)); //Name of the directory  
    const char *directory = ".";
    dir = opendir(directory); // opendir () checks ifileDescriptorirectory has NULL or not 
    // while there is another item in the directory not looked at yet 
    if(dir == NULL){
        puts("Could not open current directory");
    }
    while((dirEntry = readdir (dir)) != NULL) { 
        printf("%s\t", dirEntry->d_name); //Print out all the contents 
    }
    printf("%s","\n");
    // The function getcwd() could also be beneficial in getting the current directory  
    if(dir != NULL){
        closedir(dir);
    }
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
        puts("No such directory found\n"); 
    } 
} 
 
 
 
 
/* 
Strcmp () will be used because the user input commands will be compared to the built-in command string i.e strcmp (input[0], “clr”); it is input[0] because it is the first command 
The builtInCommands will take in the argument from the user and determines what the string input is with if-else statements in the function i.e. if(strcmp (input[0], “clr”)) { then use clear(input)} which will clear the screen.  
The type of the function is int because it is going to return a number whether it be 0 or -1, so that this function will be used more in the more complex functions such as redirection or piping 
NOTE: The inputs will be parsed before passed in the function every time.  
*/ 
int builtInCommands(char **parsedArgs){ 
    if(parsedArgs[0] == NULL){  
        // if the user doesn’t input a command that are built-in or external i.e. fileDescriptorfileDescriptor or \n 
        puts("Command doesn’t exit. Try again."); 
        return 0;   
    } 
    if(strcmp (parsedArgs[0], "quit")==0){ 
        //quit();
        puts("Exiting my shell.");
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
The checkAndExecute finds the redirection symbols from the input and runs the function if they are found.
The fileDescriptor is used to help redirect input and output in conjuction with dup() and dup2().

Unfornately, the redirection function doesn't seem to properly work with the fork() and execvp() in the function so normalExecution() was used
as a result the temp and temp2 using dup() was necessary as it could find the lowest numerical file descriptor, then choose the file descriptor with
dup2(). However, because of the normalExecution(), using multiple redirection on one line doesn't seem to work.    
*/ 

// Handles I/O redirection.
int redirection(char** args){
    
    int i, j;
    int argsNum = 0;
    int inputPosition = 0;
    int truncPosition = 0;
    int appendPosition = 0;
    int inputFile;
    int outputFile;
    int temp; 
    int temp2;
    int flag = 1;
    //int fileDescriptor0,fileDescriptor1;
    //only one file descriptor was necessary
    int fileDescriptor;
    // command of the left of redirection symbols.
    char **cmd = malloc(sizeof(char *) * 30);
    if(cmd == NULL) {
        puts("Error, no command.");
        exit(1);
    }
    //puts("1. Checking the function for segmentation fault");

    //This part of the function finds the position of the redirection symbols 
    //and increments the corresponding redirection symbol
    for(i = 0; args[i] != NULL; i++){
        //puts("2. Where is it?");
        if(args[i][0] == '<'){
            //puts("3.1 Is it here?");
            inputPosition = i;
            argsNum++;
        }
        if(args[i][0] == '>'){
            //puts("3.2 Is it here?");
            truncPosition = i;
            argsNum++;
        }
        if(args[i][0] == '>' && args[i][1] == '>'){ 
            //puts("3.3 Is it here?");
            appendPosition = i;
            argsNum++;
        }

        //Use strcpy to copy command to the left of symbols to buffer as cmd so that left of the 
        //first redirection symbol can be used for execution.
        if(argsNum == 0) {
            cmd[j] = (char *) malloc(sizeof(char *) * strlen(args[i]) + 1);
            if(cmd[j] == NULL) {
                puts("Error, no command elements");
                exit(1);
            }
            strcpy(cmd[j], args[i]);
            j++;
        }
    }

    //This part of the function handles the redirection corresponding to the respective 
    //redirection symbols and executes the command

    // if only >> symbol
    if(appendPosition != 0) {
        outputFile = appendPosition + 1;
        // If file descriptor won't open
        if((fileDescriptor = open(args[outputFile], O_WRONLY|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO)) == -1){
            perror("Error, input file failed to open.");
            exit(1);
        }
        temp = dup(1); //Finds the lowest file descriptor
        dup2(fileDescriptor, 1);
        close(fileDescriptor);    

        normalExecution(cmd);
        dup2(temp, 1);
        close(temp);
    }
    // if only ">" symbol 
    else if(truncPosition != 0) {
        outputFile = truncPosition + 1;
        if((fileDescriptor = open(args[outputFile], O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO)) == -1){
            perror("Error, input file failed to open.");
            exit(1);
        }
        temp = dup(1);
        dup2(fileDescriptor, 1);
        close(fileDescriptor);    

        normalExecution(cmd);
        dup2(temp, 1);
        close(temp);
        
    }
    // if only the "<" symbol
    
    else if(inputPosition != 0) {
        //puts("In input redirection if statement");
        inputFile = inputPosition + 1;
        if((fileDescriptor = open(args[inputFile], O_RDONLY, 0)) == -1){
            perror("Error, input file failed to open.");
            exit(0);
        }
        temp = dup(0);
        dup2(fileDescriptor, 0);
        close(fileDescriptor);    

        normalExecution(cmd);
        dup2(temp, 0);
        close(temp);
        
    }

    // if < and > symbols on one line
    else if(inputPosition != 0 && truncPosition != 0) {
        inputFile = inputPosition + 1;
        outputFile = truncPosition + 1;

        if((fileDescriptor = open(args[inputFile], O_RDONLY,0)) == -1) {
            puts("Error input file descriptor not opened");
            return;
        }
        temp = dup(0);
        dup2(fileDescriptor, 0);
        close(fileDescriptor);

        if((fileDescriptor = open(args[outputFile], O_RDWR | O_CREAT | O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO)) == -1) {
            puts("Error output truncation file descriptor not opened");
            return;
        }
        temp2 = dup(1);
        dup2(fileDescriptor, 1);
        close(fileDescriptor);

        normalExecution(cmd);

        dup2(temp, 0);
        close(temp);
        dup2(temp2, 1);
        close(temp2);

    
    } 
    // if < and >> are on the same line
    else if(inputPosition != 0 && appendPosition != 0){
        inputFile = inputPosition + 1;
        outputFile = appendPosition + 1;

        if((fileDescriptor = open(args[inputFile], O_RDONLY,0)) == -1) {
            puts("Error input file descriptor not opened");
            exit(0);
        }
        temp = dup(0);
        dup2(fileDescriptor, 0);
        close(fileDescriptor);

        if((fileDescriptor = open(args[outputFile], O_RDWR | O_CREAT | O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO)) == -1) {
            puts("Error output append file descriptor not opened");
            exit(0);
        }
        temp2 = dup(1);
        dup2(fileDescriptor, 1);
        close(fileDescriptor);

        normalExecution(cmd);
        
        dup2(temp, 0);
        close(temp);
        dup2(temp2, 1);
        close(temp2);
    }

     
    else {
        return;
    }
    return 0;
}

 
/* 
The function for pipe () parameter [0] will be the file descriptor for the read end of pipe. Parameter [1] will be the file descriptor for the write end of pipe 
Just like redirection, the function will be an int so it will return 0 on success or return -1 on failure. 
The functions fork(), wait() which is process waiting for the other process to finish, and execvp() might be used 
whileloop is used to find the “|” pipe symbol 
*/ 
//int piping(char *input){ 
int piping(char **args){ 
//First pares the input. 
    //char **args = parser(input); 
    int index = 0; 
    char *first[30], *second[30], *pipeSym[1]; // assign the first and second parameters when finding the pipe symbol 
    pid_t pid1, pid2; //Process IDs for first argument and second argument respectively
    pid_t pid;
    //int pid;
    // an array containing the input and output file descriptors
    // pipe_file_descs[0] -> read from this
    // pipe_file_descs[1] -> write to this 
    int pipeFileDesc[2]; //File descriptors
    
    int pipeIndex = 0;

    int size;
    for(size = 0; args[size] != NULL; size++);

    // com1 com2 com3(|) com4 com5
    for(pipeIndex = 0; pipeIndex < size; pipeIndex++){
        if(args[pipeIndex][0] == '|'){
            break;
        }
    }
    int i = 0;
    for(i; i < pipeIndex; i++){
        first[i] = args[i];
    }
    first[i] = NULL;
    int j = 0;
    int k = pipeIndex+1;
    for(j; k < 5;k++){
        second[j] = args[k];
        j++;
    }
    second[j] = NULL;




if (pipe(pipeFileDesc) == -1){
        puts("Failed to create pipe!");
        exit(-1);
    }

        // fork first child 
    if ((pid = fork()) < 0){
        puts("fork 1 failed!\n");
        exit(-1);
    }

    if(pid == 0) { // child 
        // close read side of pipe
        close(pipeFileDesc[0]);
        // redirectionect stdout to write side of pipe
        dup2(pipeFileDesc[1], 1);
        // exec program 1 
        if (execvp(first[0],first) < 0){
        puts("Could not execute command 1");
        exit(-1);
        }
    }

    else { // parent
        waitpid(pid, NULL, 0);
    }

    //NEXT PROCESS
    // fork second child
    if ((pid = fork()) < 0){
        puts("Second fork failed");
        exit(-1);
    }

    if(pid == 0) { // child 
        // close write side of pipe
        close(pipeFileDesc[1]);
        // redirectionect stdin to read side of pipe
        dup2(pipeFileDesc[0], 0);
        // exec program 2
        if (execvp(second[0],second) < 0){
        puts("Could not execute command 2");
        exit(-1);
        }
    }

    else { // parent
        // IMPORTANT: close both pipe fileDescriptors BEFORE waiting on the final
        // process, or else your child will block indefinitely, thinking
        // more is going to come through the pipe!
        close(pipeFileDesc[0]);
        close(pipeFileDesc[1]);
        // wait on 2nd processx
        waitpid(pid, NULL, 0);
    }



return 0; // return 0 ifsuccessful 
} 



//int normalExecution(char *input){
int normalExecution(char **args){
    //First pares the input. 
    //char **args = parser(input); 
    int index = 0; 
    char **normCommand[30];
    char **path = args[0];
    
    while(args[index] != NULL){
        normCommand[index] = args[index];
        index++;
    }
    normCommand[index] = NULL;

    //my shell built-in commands work
    if(builtInCommands(args) == 0){
        return 0;
    }

    pid_t pid = fork();

    //If pid is 0 or greater then it has successfully created a child process 
    if(pid>=0){ //CHILD Process 
        if(pid == 0){
            if(execvp(path,normCommand) < 0){
                puts("\nCommand not found in my shell.");
            }
        }
        else{ //PARENT Process
            //Wait for child process to finish executing 
            waitpid(pid,NULL,0);
        }
    }

    else{
        puts("Forking Error!\n");
    }
    return 0;
}


//using ping localhost &
//Can only do one background execution
//The function is very similar to normalExecution(), except that parent process will not wait
int backgroundExecution(char **args){
    int index = 0;
    //char **args = parser(input);
    char *path = args[0];
    char *backgroundCommand[50];
    
    //char *cmd[50]; // testing the theory
    int status;
    int size; 
    for(size=0; args[size]!=NULL; size++);
    if(builtInCommands(args) == 0){
        return 0;
    }
    //while(args[index][0] != '&'){
        /*if(args[index][0] == '&'){
            backgroundCommand[index] = args[index];
            //args[index] = '\0';
            //backgroundCommand[index] = args[index];
            index++;
            break;
        }*/

      //  backgroundCommand[index] = args[index];
        //index++;
    //}
    printf("\nSize of array: %d\n", size);
    int bgIndex;
    for(bgIndex = 0; bgIndex < size; bgIndex++){
        if(args[bgIndex][0] == '&'){
            break;
            
            /*
            pid = fork()
            if(pid==0){
                if(execvp(args[0],args))
            }
            */
        }
    }
    int commandIndex = 0;

    for(commandIndex; commandIndex < bgIndex; commandIndex++){
        backgroundCommand[commandIndex] = args[commandIndex];
    }
    backgroundCommand[commandIndex] = NULL;

    //printf("INDEX %d: %s\n", index, backgroundCommand[index]);
    
    
    pid_t pid = fork();
    printf("process id [%d]\n", pid);
    /*
    Used the same method of forking from the normal execution.
    However, there is no wait() for the background execution since it will
    try to happen at the same time
    */

    //If pid is 0 or greater then it has successfully created a child process 
    if(pid >= 0){ //CHILD Process 
        if(pid == 0){
            sleep(1);
            if(execvp(backgroundCommand[0],backgroundCommand) == -1){
                perror("\nCommand not found in my shell.");
            //exit(-1);
            }
        }
        /*else{ //PARENT Process
            //Wait for child process to finish executing 
           //int status = 0;
            waitpid(pid,NULL,0);
        }*/
    }

    else{
        perror("Forking Error!\n");
        //exit(-1);
        //return 1;
    }

    return 0;
}





/*
Unfortunately the batchfile doesn't seem to work properly with the normalExecution() function in the checkAndExectue() function.
While echo kinda works with the first character missing, the redirection with files do work. However, it seems the external commands
and built-in commands are not working.  
*/
void getBatchfile(char *fileName){
    char input[1024];
    int exitFlag = 0;
    FILE *filePtr = fopen(fileName, "r");
        if(filePtr == NULL){
            puts("Error in opening file");
            return 0;
        }
        
        while(fgets(input, 1024, filePtr) && exitFlag == 0){
            printf("string: %s", input); //Checking the commands passed into the 
            if(input[strlen(input)-1] == '\n'){
                input[strlen(input)-1] = '\0'; //null terminate command on each line
                char **args = parser(input);
                exitFlag = checkAndExecute(args); 
            }
            if(strcmp(input, "quit") == 0){
                exit(0);
            }
            
        }
        fclose(filePtr);
}


// Handles the command and then executes them.
int checkAndExecute(char **input) {
  // Check for symbols
  int pipe = 0;
  int inputredirection = 0;
  int outputTrunc = 0;
  int outputAppend = 0; 
  int backgroundEx = 0;
  int i;

	for(i = 0; input[i] != NULL; i++) {
	  //if(strcmp(input[i], "|") == 0) {
      if(input[i][0] == '|') {
		pipe++;
	  }
      //else if(strcmp(input[i], "<") == 0){
      else if(input[i][0] == '<') {
		inputredirection++;
	  }
      //else if(strcmp(input[i], ">") == 0){
      else if(input[i][0] == '>') {
		outputTrunc++;
	  }
      
      //else if(strcmp(input[i], ">>") == 0){
      else if(input[i][0] == '>' && input[i][1] == '>') {
		outputAppend++;
	  }
      //else if(strcmp(input[i], "&") == 0){}
      else if(input[i][0] == '&'){
		backgroundEx++;
	  }

	}

	// Command only contains I/0 redirection.
	if((outputAppend != 0 || outputTrunc != 0 || inputredirection != 0) && backgroundEx == 0 && pipe == 0) {
	  int status = redirection(input);
      if(status != 0){
          puts("redirection failed");
      }
	  
	}
    // Command only contains pipinng 
    else if(pipe != 0 && backgroundEx == 0) {
		int status = piping(input);
        if(status != 0){
            puts("Piping Failed");
        }
	  
	}
    // Command is a background process.  
    else if(backgroundEx != 0) {
		int status = backgroundExecution(input);
        if(status != 0){
            puts("Background execution failed");
        }
    } 
    // If command does not have redirection, piping, and no background execution.
	 else {
	  // Checks if built in command, if not it is an external command.
	  if(input[0] == NULL){
		return 0;
	  } 
      else if(builtInCommands(input) == 0){
          return 0;
      }
      else{
          int status = normalExecution(input);
          if(status != 0){
              puts("Execution failed\n");
          }
        } 
	}
  return 0;
}


int main(int argc, char *argv[]){
    if(argc == 2){
        getBatchfile(argv[1]);
    }
    else if (argc < 2){
        puts("\n------MY SHELL------\n");
        char input[1024];
        int exitFlag = 0;
        shellPrompt(input);
        
        while(exitFlag == 0){
            char **args = parser(input);
            exitFlag = checkAndExecute(args);
            shellPrompt(input);
        }
    }
    return 0;
}