#include "execute.h"

void execute_program(char* program_name, char** program_args) {
    int fd = open("fifo", O_WRONLY);
    struct mensagem msg_antes;
    struct mensagem msg_depois;

    struct timeval start_anonimo;

    // cria pipe anonimo para enviar tempo antes de executar
    int pipe_anonimo[2];

    if (pipe(pipe_anonimo) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

    int pid_fork = fork();

    if (pid_fork == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid_fork == 0) {

        close(pipe_anonimo[0]);  // close the read end of the pipe

        //envia o pid para o servidor
        msg_antes.pid = getppid();

        printf("Process ID: %d\n", msg_antes.pid);


        //envia o nome do programa para o servidor
        strncpy(msg_antes.name_program, program_name, sizeof(msg_antes.name_program));

        //envia o timestamp atual para o servidor
        msg_antes.type = 1;
        gettimeofday(&msg_antes.tempo, NULL);

        write(fd, &msg_antes, sizeof(msg_antes));
        write(pipe_anonimo[1], &msg_antes.tempo, sizeof(msg_antes.tempo));
        close(pipe_anonimo[1]);  // close the write end of the pipe

        execvp(program_name, program_args);

        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        close(pipe_anonimo[1]);  // close the write end of the pipe

        int status;
        pid_t child_pid = wait(&status);

        gettimeofday(&msg_depois.tempo, NULL);
        
        read(pipe_anonimo[0], &start_anonimo, sizeof(msg_antes.tempo));
        close(pipe_anonimo[0]);  // close the read end of the pipe
        
        pid_t pai = getpid();
        msg_depois.pid = pai;
        strncpy(msg_depois.name_program, program_name, sizeof(msg_depois.name_program));
        msg_depois.type = 1;
        write(fd, &msg_depois, sizeof(msg_depois));
    
        long elapsed_microseconds = msg_depois.tempo.tv_usec - start_anonimo.tv_usec;
        printf("Program '%s' with PID %d ran for %ld microseconds\n", program_name, msg_depois.pid, elapsed_microseconds);

    }
}


int main(int argc, char* argv[]) {
    if (argc >= 4 && !strcmp(argv[1], "execute") && !strcmp(argv[2], "-u")) {
        char* program_name = argv[3];
        char* program_args[argc - 2];
        for (int i = 3; i < argc; i++) {
            program_args[i - 3] = argv[i];
        }
        program_args[argc - 3] = NULL;

        execute_program(program_name, program_args);
    }
    return 0;
}