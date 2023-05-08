#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>

int main (int argc, char* argv[]) {

    if (argc >= 4 && !strcmp(argv[1], "execute") && !strcmp(argv[2], "-u")) {
        int fd = open ("fifo", O_WRONLY);

       
        char* args[argc-2];
        for (int i = 3; i < argc; i++) {
            args[i-3] = argv[i];
        }
        args[argc-3] = NULL;

        struct timeval tv_start, tv_end, tv_beforefunc;

        gettimeofday(&tv_start,NULL);
        
        int pid_fork = fork();
        
        if (pid_fork == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid_fork == 0) {

        pid_t pid = getpid();
        
        //envia o pid para o servidor
        write (fd, &pid, sizeof(pid)); 

        //envia o nome do programa para o servidor
        write(fd, argv[3], strlen(argv[3]) + 1);

        printf("Process ID: %d\n", pid);

        //envia o timestamp atual para o servidor
        gettimeofday(&tv_beforefunc,NULL);
        write (fd,&tv_beforefunc,sizeof(tv_beforefunc));


        execvp(argv[3], args);

        perror("execvp");
        exit(EXIT_FAILURE);
        } else {
            wait (NULL);
            gettimeofday(&tv_end, NULL);
            
            printf("Forked child with PID: %d\n", pid_fork);

            //timersub(&tv_end, &tv_start, &tv_diff);
            //printf("Time elapsed: %ld.%06lds\n", tv_diff.tv_sec, tv_diff.tv_usec);
            long elapsed_milliseconds =  (tv_end.tv_usec - tv_start.tv_usec);                      
            printf("Time elapsed: %ld micro seconds\n", elapsed_milliseconds);
        }
    } 

    return 0;
}