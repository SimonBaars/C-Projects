#include "arena.h"
#include "ast.h"
#include "shell.h"
#include "front.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <string.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

int currentProcess = -1;
int* currentChilds = NULL; 
int nCurrentChilds = 0; 

void addPIDToCurrentChilds(int pid){ 
    //int* currentChildsOld = currentChilds; 
    if(currentChilds == NULL)
        currentChilds = malloc(1);
    currentChilds = realloc(currentChilds, (nCurrentChilds+2)*sizeof(int)); 
    /*if(currentChildsOld!=NULL){ 
        int i = 0; 
        for(i = 0; i<nCurrentChilds; i++){ 
            currentChilds[i] = currentChildsOld[i]; 
        } 
        free(currentChilds); 
    } */
    currentChilds[nCurrentChilds] = pid; 
    nCurrentChilds++; 
} 

struct queueItem {
            char *program;
            char **argv;
            size_t argc;
        } queueItem;

void my_free_tree(void *pt) {
    free_tree((node_t *) pt);
}

void eventHandler(int signalNumber) {
    if (currentProcess != -1 && signalNumber != SIGCHLD) {
        if (signalNumber == SIGINT) kill(currentProcess, SIGKILL);
        else if (signalNumber == SIGTSTP) kill(currentProcess, SIGTSTP);
        currentProcess = -1;
    }
}

void initialize(void) {
    struct sigaction psa;
    psa.sa_handler = &eventHandler;
    sigemptyset(&psa.sa_mask);
    sigaddset(&psa.sa_mask, SIGINT);
    psa.sa_flags = SA_NOCLDWAIT;
    sigaction(SIGTSTP, &psa, NULL);
    sigaction(SIGINT, &psa, NULL);
}

void run_command(node_t *node) {
    // Create a new memory arena to handle this function in. Please see the
    // hints and `arena.h` on how to use them, as you can very easily create
    // memory leaks if you are not careful.
    arena_push();
    arena_register_mem(node, &my_free_tree);

    // (for testing:)
    //print_tree(node);
    showFD(parseNode(node, -1, true));
    
    int i; 
    for(i = 0; i<nCurrentChilds; i++){ 
        handleResult(currentChilds[i]); 
    } 
    free(currentChilds); 
    currentChilds = NULL; 
    nCurrentChilds = 0; 

    // Pop the current 1memory arena, this frees all memory in the current arena.
    arena_pop();
}

void showFD(int writePntr) {
    if(writePntr!=-1){
        char readbuffer[1];
        while (read(writePntr, readbuffer, 1) > 0) {
            printf(readbuffer);
        }
        close(writePntr);
    }
}

void executeCommand(command_t command) {
    char *program = command.program;
    char **argv = command.argv;

    execvp(program, argv);
    perror("Execution error.");
    raise(SIGKILL);
}

void executeCatCommand() {
    char *cmd = "cat";
    char *argv[2];
    argv[0] = "cat";
    argv[1] = "/dev/null";

    execlp(cmd, argv[0], argv[1], NULL);
    exit(1);
}

void writePointersToPipe(int pointer1, int pointer2, int fd[]){
    char readbuffer[1];
    if (pointer1 != -1) {
        while (read(pointer1, readbuffer, 1) > 0) {
            write(fd[PIPE_WRITE], readbuffer, 1);
        }
    }
    
    if (pointer2 != -1) {
        while (read(pointer2, readbuffer, 1) > 0) {
            write(fd[PIPE_WRITE], readbuffer, 1);
        }
    }
}

int handleSequenceNode(node_t* node, int writePntr, bool doWait) {
    int pointer1 = parseNode(node->sequence.first, writePntr, true);
    if(doWait) showFD(pointer1);
    int pointer2 = parseNode(node->sequence.second, writePntr, doWait);
    if(doWait) showFD(pointer2);
    
    if(doWait)
        return -1;

    int fd[2];
    pid_t childpid;
    pipe(fd);
    if ((childpid = fork()) == -1)
        perror("fork");

    if (childpid == 0) {
        dup2(fd[PIPE_WRITE], STDOUT_FILENO); // send stdout to the pipe
        writePointersToPipe(pointer1, pointer2, fd);
        
        close(fd[PIPE_WRITE]);
        close(fd[PIPE_READ]);
        executeCatCommand();
    } else {
        if (pointer1 != -1) close(pointer1);
        if (pointer2 != -1) close(pointer2);
        close(fd[PIPE_WRITE]);
        writePntr = fd[PIPE_READ];
    }
    return writePntr;
}

int handlePipe(node_t* node, int writePntr) {
    node_t** parts = node->pipe.parts;
    unsigned int i;
    for (i = 0; i < node->pipe.n_parts; i++) {
        writePntr = parseNode(parts[i], writePntr, false);
    }
    return writePntr;
}

void handleResult(int processId) {
    currentProcess = processId;
    int status;
    waitpid(processId, &status, WUNTRACED);
    currentProcess = -1;
}

int handleCommand(node_t* node, int writePntr, bool doWait) {
    if (strcmp(node->command.program, "exit") == 0) {
        if (doWait) {
            if (node->command.argc > 1) {
                exit(atoi(node->command.argv[1]));
            } else {
                exit(1);
            }
        } else return -1;
        raise(SIGKILL);
    } else if (strcmp(node->command.program, "cd") == 0) {
        if (doWait && chdir(node->command.argv[1]) == -1)
            perror("Something went wrong while changing directories!");
        else return -1;
        raise(SIGKILL);
    } else {
        writePntr = handlePipeCommand(node->command, writePntr, doWait);
    }
    return writePntr;
}

int handlePipeCommand(command_t command, int writePntr, bool doWait) {
    int fd[2];
    pid_t childpid;
    pipe(fd);
    if ((childpid = fork()) == -1) {
        perror("fork");
    }
    if (childpid == 0) {
        if (writePntr >= 0) {
            dup2(writePntr, STDIN_FILENO);
        } else close(fd[0]);
        dup2(fd[PIPE_WRITE], STDOUT_FILENO); // send stdout to the pipe
        if (writePntr >= 0) {
            close(writePntr);
        }
        if (writePntr < 0) {
            close(fd[PIPE_WRITE]);
        }
        executeCommand(command);
    } else {
        close(fd[PIPE_WRITE]);
        writePntr = fd[PIPE_READ];
    }
    if (doWait) handleResult(childpid);
    else addPIDToCurrentChilds(childpid);
    return writePntr;
}

int handleDetach(node_t* node, int writePntr) {
    pid_t childpid;
    if ((childpid = fork()) == -1) {
        perror("fork");
    }
    if (childpid == 0) {
        int pntr = parseNode(node->detach.child, writePntr, true);
        showFD(pntr);
        raise(SIGKILL);
    }
    return writePntr;
}

int handleSubshell(node_t* node, int writePntr, bool doWait) {
    int fd[2];
    pid_t childpid;
    pipe(fd);
    if ((childpid = fork()) == -1) {
        perror("fork");
    }
    if (childpid == 0) {
        dup2(fd[PIPE_WRITE], STDOUT_FILENO); // send stdout to the pipe
        char readbuffer[1];

        int pntr = parseNode(node->subshell.child, writePntr, doWait);
        if (pntr != -1) {
            while (read(pntr, readbuffer, 1) > 0) {
                write(fd[PIPE_WRITE], readbuffer, 1);
            }
            close(pntr);
        }

        close(fd[PIPE_WRITE]);
        close(fd[PIPE_READ]);
        executeCatCommand();
    } else {
        close(fd[PIPE_WRITE]);
        writePntr = fd[PIPE_READ];
    }
    if (doWait) handleResult(childpid);
    return writePntr;
}

int parseNode(node_t* node, int writePntr, bool doWait) {
    switch (node->type) {
        case NODE_COMMAND:
            return handleCommand(node, writePntr, doWait);
        case NODE_SEQUENCE:
            return handleSequenceNode(node, writePntr, doWait);
        case NODE_PIPE:
            return handlePipe(node, writePntr);
        case NODE_DETACH:
            return handleDetach(node, writePntr);
        case NODE_SUBSHELL:
            return handleSubshell(node, writePntr, doWait);
        default:
            perror("Node type not implemented");
            break;
    }
    return -1;
}