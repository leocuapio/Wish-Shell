#include<stdio.h> 
#include<stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h> //open
#include <string.h>

#define BUFF_SIZE  256
#define EXIT_SUCCESS 0
#define EXIT_FAIL 1

char * path[BUFF_SIZE] = {"/bin", "/usr/bin", NULL};
int paths_num = 2; 

void error();

int main (int argc, char * argv[]){
      while(1){
        printf("PROMPT>");
        FILE * stream = stdin;
        char * line = NULL;
        size_t len = 0;
        ssize_t nread;

        nread = getline(&line, &len, stream);
        printf("Retrieved line of length %zd:\n", nread);
        printf("This is the line I received:\n%s\nCool right\n", line);

        printf("NOW I WILL NEED TO REMOVE THE NEW LINE CHARACTER\n");
        int rc = fork(); 
        if (rc<0){ //check if fork failed
            printf("Fork Failed\n");
        }
        else if (rc ==0){ //Child Process
            char * args[BUFF_SIZE];
            args[0] = line;
            args[1] = NULL; // null terminated array
            fclose(stdout);
            int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            execv(args[0], args);
            error();
            exit(1); // exit the child to return back to parent. 
        }
        else{// parent process
            wait(NULL);
        }

        }
      
    
}


void error(){
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}