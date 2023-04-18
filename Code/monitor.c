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

    int fd = open ("fifo", O_RDONLY);

    pid_t pid;
    char program[100];
    struct timeval tv_start, tv_end;
    long elapsed_microseconds;

    while (1) {
        int nbytes = read(fd, &pid, sizeof(pid));
        if (nbytes <= 0) {
            // No more data available, exit the loop
            break;
        }
        read(fd, program, sizeof(program));
        read(fd, &tv_start, sizeof(tv_start));
        read(fd, &tv_end, sizeof(tv_end));
        elapsed_microseconds = tv_end.tv_usec - tv_start.tv_usec;
        printf("Program '%s' with PID %d ran for %ld microseconds\n", program, pid, elapsed_microseconds);
    }

    close(fd);

    return 0;
}
