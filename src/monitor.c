#include "execute.h"
int main () {


if (mkfifo("fifo",0666) == -1) {
        perror ("fifo:");
        exit (EXIT_FAILURE);
    }

    int fd = open ("fifo", O_RDWR);

    long elapsed_microseconds;

    struct mensagem msg_antes;
    struct mensagem msg_depois;

    while (read (fd,&msg_antes,sizeof(msg_antes)) > 0) {
        
        if (msg_antes.type == 1) {
       
            pid_t child_pid = msg_antes.pid;
            
            int status;
            waitpid(child_pid, &status, 0);

            read(fd, &msg_depois,sizeof(msg_depois));

            elapsed_microseconds = msg_depois.tempo.tv_usec - msg_antes.tempo.tv_usec;
            printf("Program '%s' with PID %d ran for %ld microseconds\n", msg_antes.name_program, msg_depois.pid, elapsed_microseconds);
        }
        else if (msg_antes.type == 2) {
            
        }


    }

    close(fd);

    return 0;
}
