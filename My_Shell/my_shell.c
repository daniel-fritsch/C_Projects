// personal shell implementation in C using syscalls.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// for wait command
#include <sys/wait.h>

// function to hand redirect commands(using > or >> characters)
void redirect_command(char *args[], char *output_filename, int append) {
    FILE *file;
    // depending on the type of oepration determined in main we either write or append based on > or >> use
    if (append) {
        file = fopen(output_filename, "a");
    } else {
        file = fopen(output_filename, "w");
    }
    if (file == NULL) {
        // error check if file input incorrect/nonexistent
        printf("Invalid File. (file does not exist) \n");
    }
    dup2(fileno(file), STDOUT_FILENO);
    fclose(file);
}

// function for handling pipe operations (there may still be some problems here)
void pipe_operation(char *args[]) {
    // break up the arguments for the pipe operations
    int pipefd[2];
    pid_t pid;
    
    // pipe error check
    if (pipe(pipefd) == -1) {
        printf("Error in pipe. \n");
        exit(EXIT_FAILURE);
    }

    // fork process and check if its ok
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);

    // read end of pipe condition
    } else if (pid == 0) {
        // redirect the stdout to the other end of pipe for execution
        close(pipefd[0]); 
        dup2(pipefd[1], STDOUT_FILENO); 
        close(pipefd[1]);

        // actually execute necessary command specified in read portion of pipe
        if (execvp(args[0], args) == -1) {
            // failure print if necessary (if fails on -1)
            printf("Error in pipe arguments. \n");
            exit(EXIT_FAILURE);
        }
    } else {

        close(pipefd[1]); 
        dup2(pipefd[0], STDIN_FILENO); 
        close(pipefd[0]); 

        // execute the command specified by args[3] to complete pipe operation
        if (execvp(args[3], args + 3) == -1) {
            // fail condition if necessarry
            printf("Error in pipe arguments. \n");
            exit(EXIT_FAILURE);
        }
    }
}

// tHis function handles external commands. It redirects necessarily to redirect and pipe functions if conditions are met(flags are set to 1)
void execute_command(char *args[], char *output_filename, int append, int pipe_flag) {
    // fork
    int execute_external_process = fork();
    if (execute_external_process < 0) {
        printf("Fork error. \n");
        exit(EXIT_FAILURE);
    } else if (execute_external_process == 0) {
        // check if conditions are met for a redirect command in whcih case I call that function
        if (output_filename != NULL) {
            redirect_command(args, output_filename, append);
        }
        // if the pipe flag is 1 we want to go to the pipe operation fucntion to handle that case
        if (pipe_flag == 1) { 
            pipe_operation(args);
        } else {
            // command error execution/check
            if (execvp(args[0], args) == -1) {
                printf("Error executing command. \n");
                exit(EXIT_FAILURE);
            }
        }
    } else {
        // wait until done
        wait(NULL);
    }
}

int main() {
    // inputs(100 total user inputs max and 10 maximum commands) to prevent massive requests
    char input[100];
    char *inputs[10];
    char *output_filename = NULL;

    // flags for specific operations
    int append = 0;
    int pipe_flag = 0;

    // overarching while loop that processes user input and continues till user "exit"
    while (1) {
        // prompt character to let the user know shell is waiting for response
        printf("> ");
        // try to read user input with fgets, else print error
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Error. fgets failed to read input. \n");
            continue;
        }

        // use strtok to tokenize/split up input. Make sure not to take out
        // > or >> or | yet because we need to check for them in case of specific cases
        char *token = strtok(input, " \t\n");
        int i = 0;
        // check for redirct write condition
        while (token != NULL) {
            if (strcmp(token, ">") == 0) {
                token = strtok(NULL, " \t\n");
                if (token != NULL) {
                    output_filename = token;
                    append = 0;
                } else {
                    printf("Missing filename after > command. No destniation. \n");
                    break;
                }
            // check for redirct append condition
            } else if (strcmp(token, ">>") == 0) {
                token = strtok(NULL, " \t\n");
                if (token != NULL) {
                    output_filename = token;
                    append = 1;
                } else {
                    printf("Missing filename after >> command. No destination \n");
                    break;
                }
            // lastly check for pipe use in which case pipe flag enabled
            } else if (strcmp(token, "|") == 0) {
                inputs[i] = NULL;
                pipe_flag = 1;
                i++;
            } else {
                inputs[i] = token;
                i++;
            }
            token = strtok(NULL, " \t\n");
        }
        inputs[i] = NULL;

        // internal commands
        // check if exit is used or cd is used and if so just fulfil those internal commands with
        // appropriate system calls. 
        if (strcmp(inputs[0], "exit") == 0) {
            printf("Exiting shell...\n");
            exit(EXIT_SUCCESS);
        } else if (strcmp(inputs[0], "cd") == 0) {
            if (chdir(inputs[1]) == -1) {
                printf("Error changing directories. \n");
            }
        } else {
            // if the command is external send that to execute_command function for processing
            execute_command(inputs, output_filename, append, pipe_flag);
        }   // inputs here are for the tokens, filename which is kind of a flag for redirect case, append also for redirect use, and pipe_flag for determining if pipe is present. 

        // reset for next user prompt/command
        output_filename = NULL;
        append = 0;
        pipe_flag = 0;
    }

    // exit gracefully
    return 0;
}
