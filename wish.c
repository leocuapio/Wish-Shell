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
void check_path(char *fullpath, char *args[]);
void builtin_check(char *args[]);

int main (int argc, char * argv[]){
      while(1){
        printf("WISH>");
        FILE * stream = stdin;
        char * line = NULL;
        size_t len = 0;
        ssize_t nread;


        char * found;
        char fullpath[BUFF_SIZE];

        nread = getline(&line, &len, stream);
        int size = strlen(line);
        line[size - 1] = '\0';

        int rc = fork(); 
        if (rc<0){ //check if fork failed
            printf("Fork Failed\n");
        }
        else if (rc ==0){ //Child Process
            char * args[BUFF_SIZE];

            // Parse user input (strsep)
            int i = 0;
            while( (found = strsep(&line," ")) != NULL ) {
                args[i] = found;
                i+=1;
            }

            check_path(fullpath, args);

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

    void error(){
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message)); 
    }

    void check_path(char *fullpath, char *args[]) {
        for (int j =0; path[j] != NULL; ++j) {
            snprintf(fullpath, BUFF_SIZE, "%s/%s", path[j],args[0]);
            if (access(fullpath, X_OK) == 0){
                args[0] = fullpath;
                break;
                }
            }
    }

    void builtin_check (char *args[]) {
        // First check for exit
        if ((strcmp(args[0], "exit") == 0) && args[1]) {
            error();
        }
        if ((strcmp(args[0], "exit") == 0) && !args[1]) {
            exit(EXIT_SUCCESS);
        }
        // Second check for cd

        // Third check for path
    }
     