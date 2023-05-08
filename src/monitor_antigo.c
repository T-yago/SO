#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>

int main () {


if (mkfifo("fifo",0666) == -1) {
        perror ("fifo:");
        exit (EXIT_FAILURE);
    }

    int fd = open ("fifo", O_RDONLY);

    pid_t pid;
    char program[100];
    struct timeval tv_start, tv_after;
    long elapsed_microseconds;



    while (1) {
        printf ("ola\n");
        read(fd, &pid, sizeof(pid));
        read(fd, program, sizeof(program));
        read(fd, &tv_start, sizeof(tv_start));
        gettimeofday (&tv_after,NULL);
            int fd_bloqueia = open ("fifo",O_WRONLY);


        elapsed_microseconds = tv_after.tv_usec - tv_start.tv_usec;
        printf("Program '%s' with PID %d ran for %ld microseconds\n", program, pid, elapsed_microseconds);
    }

    close(fd);

    return 0;
}
