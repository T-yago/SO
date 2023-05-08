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

struct mensagem {
    pid_t pid;
    char name_program [60];
    struct timeval tempo;
};

void execute_program(char* program_name, char** program_args);

#endif
