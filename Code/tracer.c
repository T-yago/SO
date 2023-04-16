#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
//int mkfifo(const char *pathname, mode_t mode);




// O ficheiro que abre para ler bloqueia até haver outro processo que abra o ficheiro no modo de escrita
int main (int argc, char* argv[]) {

    int fd = open ("fifo",O_WRONLY);


    if (!strcmp(argv[1],"execute") && !strcmp (argv[2],"-u")) { // execute -ps


    // informa o servidor do processo que vai executar o programa
    pid_t pid = getpid();
    write (fd,&pid,sizeof(pid)); 

    //O nome do programa a executar
    write (fd,argv[1],sizeof(argv[1]));


    struct timeval tv;
    gettimeofday(&tv, NULL);

    long long timestamp = (long long)tv.tv_sec * 1000000 + tv.tv_usec;

    //O timestamp atual
    write(fd, &timestamp, sizeof(timestamp));

    
    write (STDOUT_FILENO,&pid,sizeof(pid));

    char* args[argc-3];  
    
    for (int i = 0; i < argc - 3; i++) {
        args[i] = argv[i+3];
    }

    execvp(argv[1],args);

    }
    
    close(fd);


    printf("Pipe com nome 'fifo' fechado.\n");

return 0;

}