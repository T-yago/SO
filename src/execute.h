#ifndef EXECUTE_PROGRAM_H
#define EXECUTE_PROGRAM_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>

typedef struct mensagem {
    pid_t pid;
    char name_program [150];
    long tempo;
    int type; //1=exec; 2=status
} Mensagem;

typedef struct store {
    char name_program [150];
    long tempo;
} Store;

void execute_program(char* program_name, char** program_args);

#endif
