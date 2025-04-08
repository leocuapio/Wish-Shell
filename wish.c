#include<stdio.h> 
#include<stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h> //open
#include <string.h>
#include <pthread.h>

#define BUFF_SIZE  256
#define EXIT_SUCCESS 0
#define EXIT_FAIL 1

// Global var of the path vars
char * path[BUFF_SIZE] = {"/bin", "/usr/bin", NULL};
int paths_num = 2; 

// Function Declarations
void error();
void check_path(char *fullpath, char *args[]);
int builtin_check(char *args[], char *path[], int paths_num);
void redirection_check(char *args[], int *check_redirection, char ** redirection_out);

int main (int argc, char * argv[]){
    //Infinite loop that spits out WISH> 
    while(1){
        printf("WISH>");
        FILE * stream = stdin;
        char * line = NULL;
        size_t len = 0;
        ssize_t nread;

        //args for the execv
        char * args[BUFF_SIZE];

        //Vars for the parsing of input
        char * found;
        char fullpath[BUFF_SIZE];

        //Getting line from input and placing it in line
        nread = getline(&line, &len, stream);

        // Getting rid of the \n at the end
        int size = strlen(line);
        line[size - 1] = '\0';


        //Clear the args array specifically if builtin cmds
        memset(args, 0, sizeof(args)); 

        // char *parallel_args [BUFF_SIZE];
        // int j = 0;
        // if (strstr(line, "&") == 0) {
        //     while( (found = strsep(&line,"&")) != NULL ) {
        //         parallel_args[j] = found;
        //         j += 1;
        //     }
        // }  

        // Parse user input (strsep)
        int i = 0;
        while( (found = strsep(&line," ")) != NULL ) {
            if (strcmp(" ", found)==0) {
                continue;
            }
            args[i] = found;
            i+=1; 
        }



        //Calling the builtin_check function to see if there are any built-in commands
        if (builtin_check(args, path, paths_num)) {
            continue;
        }

        //Redirection varss and check
        char *redirection_out = NULL;
        int check_redirection = 0;

        redirection_check(args, &check_redirection, &redirection_out);


        int rc = fork(); 
        if (rc<0){ //check if fork failed
            printf("Fork Failed\n");
        }
        else if (rc ==0){ //Child Process
            check_path(fullpath, args);

            if (check_redirection) {
                int fd = open(redirection_out, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            execv(args[0], args);
            error();
            exit(1); // exit the child to return back to parent. 
        }
        else{// parent process
            wait(NULL);
        }
      }
    }


    //Functions 

    //Error function as described in github
    void error(){
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message)); 
    }
    
    //Used to check the path of the command and if it can be accessed modify the command
    void check_path(char *fullpath, char *args[]) {
        for (int j =0; path[j] != NULL; ++j) {
            snprintf(fullpath, BUFF_SIZE, "%s/%s", path[j],args[0]);
            if (access(fullpath, X_OK) == 0){
                args[0] = fullpath;
                break;
                }
            }
    }

    //Built_in command check that checks for exit, cd, and path and handles them accordingly
    int builtin_check (char *args[], char* paths[], int paths_num) {
        // First check for exit
        if ((strcmp(args[0], "exit") == 0)) {
            if (args[1] == NULL) {
                exit(0);
            } else {
                error();
                return 1;
            }
        }
        // Second check for cd
        else if ((strcmp(args[0], "cd") == 0)) {
            if (args[1] && !args[2]) {
                if (chdir(args[1]) != 0) {
                    error();
                }
            } else {
                error();
            }
            return 1;
        }
        // Third check for path
        // else if ((strcmp(args[0], "path") == 0)) {
        //     memset(args, 0, sizeof(args)); 
        //     for (int i =0; paths[i] != NULL; ++i) {

        //     }
        // }

        return 0;
    }

    // Redirection check and modifys args array accordingly
    void redirection_check(char * args[], int *check_redirection, char ** redirection_out) {
        for (int l = 0; args[l] != NULL; ++l) {
            if (strcmp(">", args[l]) == 0) {
                if (args[l+1] == NULL) {
                    error();
                }
                *check_redirection = 1;
                args[l] = NULL;
                *redirection_out = args[l+1];
            }
        }
    }
     