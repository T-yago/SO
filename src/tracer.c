#include "execute.h"

#define MAX_COMMANDS 20
#define MAX_ARGS 10

void execute_pipeline(char* pipeline)
{
    int fd = open("fifo", O_WRONLY);
    struct mensagem msg_antes;
    struct mensagem msg_depois;

    msg_antes.pid = getpid();

    char str_Pid_Message[60];
    sprintf(str_Pid_Message, "Running PID %d\n", msg_antes.pid);
    write (1, &str_Pid_Message, strlen(str_Pid_Message)+1);

    // Envia o nome do programa para o servidor

    strcpy(msg_antes.name_program, pipeline);

    // Envia o timestamp atual para o servidor

    msg_antes.type = 1;
    struct timeval tempo_Aux;
    gettimeofday(&tempo_Aux, NULL);
    msg_antes.tempo = tempo_Aux.tv_sec * 1000000L + tempo_Aux.tv_usec;

    write(fd, &msg_antes, sizeof(msg_antes));

    int num_cmds = 0;
    char* cmds[MAX_COMMANDS][MAX_ARGS];

    char* cmd_token = NULL;
    char* saveptr = NULL;
    cmd_token = strtok_r(pipeline, "|", &saveptr);
    while (cmd_token != NULL && num_cmds < MAX_COMMANDS) {
        int num_args = 0;
        char* arg_token = strtok(cmd_token, " ");
        while (arg_token != NULL && num_args < MAX_ARGS) {
            cmds[num_cmds][num_args++] = arg_token;
            arg_token = strtok(NULL, " ");
        }
        cmds[num_cmds][num_args] = NULL; // marca o final dos argumentos
        num_cmds++;
        cmd_token = strtok_r(NULL, "|", &saveptr);
    }

    int pipes[num_cmds-1][2]; // cria pipes para cada conexão entre os comandos

    // cria todos os pipes necessários
    for (int i = 0; i < num_cmds-1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // executa cada comando em um processo filho
    for (int i = 0; i < num_cmds; i++) {
        int pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // processo filho

            // redireciona entrada padrão se não for o primeiro comando
            if (i > 0) {
                if (dup2(pipes[i-1][0], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // redireciona saída padrão se não for o último comando
            if (i < num_cmds-1) {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // fecha os pipes não usados
            for (int j = 0; j < num_cmds-1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // executa o comando
            if (execvp(cmds[i][0], cmds[i]) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }
    }

    // fecha todos os pipes no processo pai
    for (int i = 0; i < num_cmds-1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // espera todos os filhos terminarem
    for (int i = 0; i < num_cmds; i++) {
        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
    }

    msg_depois.pid = getpid();
    strcpy(msg_depois.name_program, msg_antes.name_program);
    msg_depois.type = 0;    // 0 para identificar que o programa terminou
    gettimeofday(&tempo_Aux, NULL);
    msg_depois.tempo = tempo_Aux.tv_sec * 1000000L + tempo_Aux.tv_usec;
    write(fd, &msg_depois, sizeof(msg_depois));

    long elapsed_microseconds = msg_depois.tempo - msg_antes.tempo;

    char str_Output[200];
    sprintf(str_Output ,"Ended in %ld miliseconds\n", elapsed_microseconds / 1000);
    write (1, &str_Output, strlen(str_Output)+1);
}

void get_status_uniq (char** program_args)
{
    // Criar o fifo que vai receber as mensagens do servidor

    char str_Pid_Receber[60];
    sprintf(str_Pid_Receber, "%d_Monitor_To_Tracer", getpid());

    if (mkfifo(str_Pid_Receber,0666) == -1) {
        perror ("Fifo Error");
        exit (EXIT_FAILURE);
    }

    // Enviar uma mensagem a sinalizar que é um stats-time (tipo==5)

    int fd_Sinalizar = open("fifo", O_WRONLY);

    Mensagem msg_Sinalizar;
    msg_Sinalizar.type = 5;
    msg_Sinalizar.pid = getpid();
    write(fd_Sinalizar, &msg_Sinalizar, sizeof(Mensagem));

    // Abre o fifo que vai receber as mensagens do servidor

    int fd_Receber = open(str_Pid_Receber, O_RDONLY);

    // Abre o fifo que vai enviar as mensagens para o servidor

    char str_Pid_Enviar[60];
    sprintf(str_Pid_Enviar, "%d_Tracer_To_Monitor", getpid());
    int fd_Enviar = open(str_Pid_Enviar, O_WRONLY);

    // Enviar para o servidor os PIDs dos programas que quer ver as informações

    for (int i = 0;program_args[i]!=NULL;i++) {
        int pid = atoi(program_args[i]);
        write (fd_Enviar, &pid, sizeof(int));
    }

    close(fd_Enviar);

    // Imprimir os nomes dos programas sem repetir nomes

    char nome[150];
    char str_Output[151];

    ssize_t bytes;
    while ((bytes = read(fd_Receber, &nome, 60 * sizeof(char)))>0) {
        sprintf (str_Output, "%s\n", nome);
        write (1, &str_Output, strlen(str_Output)+1);
    }

    close (fd_Receber);
}

void get_status_command (char* program_name, char** program_args)
{
    // Criar o fifo que vai receber as mensagens do servidor

    char str_Pid_Receber[60];
    sprintf(str_Pid_Receber, "%d_Monitor_To_Tracer", getpid());

    if (mkfifo(str_Pid_Receber,0666) == -1) {
        perror ("Fifo Error");
        exit (EXIT_FAILURE);
    }

    // Enviar uma mensagem a sinalizar que é um stats-time (tipo==4)

    int fd_Sinalizar = open("fifo", O_WRONLY);

    Mensagem msg_Sinalizar;
    strcpy (msg_Sinalizar.name_program, program_name);
    msg_Sinalizar.type = 4;
    msg_Sinalizar.pid = getpid();
    write(fd_Sinalizar, &msg_Sinalizar, sizeof(Mensagem));

    // Abre o fifo que vai receber as mensagens do servidor

    int fd_Receber = open(str_Pid_Receber, O_RDONLY);

    // Abre o fifo que vai enviar as mensagens para o servidor

    char str_Pid_Enviar[60];
    sprintf(str_Pid_Enviar, "%d_Tracer_To_Monitor", getpid());
    int fd_Enviar = open(str_Pid_Enviar, O_WRONLY);

    // Enviar para o servidor os PIDs dos programas que quer ver as informações

    for (int i = 0;program_args[i]!=NULL;i++) {
        int pid = atoi(program_args[i]);
        write (fd_Enviar, &pid, sizeof(int));
    }

    close(fd_Enviar);

    // Receber as respostas

    int ocorrencias = 0;
    int occorreu;

    ssize_t bytes;
    while ((bytes = read(fd_Receber, &occorreu, sizeof(int)))>0) {
        ocorrencias++;
    }

    close (fd_Receber);

    // Escrever o output

    char str_Output[200];
    sprintf(str_Output, "%s was executed %d times\n", program_name, ocorrencias);
    write(1, &str_Output, strlen(str_Output)+1);
}

void get_status_time(char** program_args)
{
    // Criar o fifo que vai receber as mensagens do servidor

    char str_Pid_Receber[60];
    sprintf(str_Pid_Receber, "%d_Monitor_To_Tracer", getpid());

    if (mkfifo(str_Pid_Receber,0666) == -1) {
        perror ("Fifo Error");
        exit (EXIT_FAILURE);
    }

    // Enviar uma mensagem a sinalizar que é um stats-time (tipo==3)

    int fd_Sinalizar = open("fifo", O_WRONLY);

    Mensagem msg_Sinalizar;
    msg_Sinalizar.type = 3;
    msg_Sinalizar.pid = getpid();
    write(fd_Sinalizar, &msg_Sinalizar, sizeof(Mensagem));

    // Abre o fifo que vai receber as mensagens do servidor

    int fd_Receber = open(str_Pid_Receber, O_RDONLY);

    // Abre o fifo que vai enviar as mensagens para o servidor

    char str_Pid_Enviar[60];
    sprintf(str_Pid_Enviar, "%d_Tracer_To_Monitor", getpid());
    int fd_Enviar = open(str_Pid_Enviar, O_WRONLY);

    // Enviar para o servidor os PIDs dos programas que quer ver as informações

    for (int i = 0;program_args[i]!=NULL;i++) {
        int pid = atoi(program_args[i]);
        write (fd_Enviar, &pid, sizeof(int));
    }

    close(fd_Enviar);

    // Receber as respostas

    long tempoTotal = 0;
    long tempoParcial;

    ssize_t bytes;
    while ((bytes = read(fd_Receber, &tempoParcial, sizeof(long)))>0) {
        tempoTotal += tempoParcial;
    }

    close (fd_Receber);

    // Escrever o output

    char str_Output[200];
    sprintf(str_Output, "Total execution time is %ld ms\n", tempoTotal);
    write(1, &str_Output, strlen(str_Output)+1);
}

void get_status()
{
    // Criar o fifo que vai receber as mensagens do servidor

    char str_Pid[60];
    sprintf(str_Pid, "%d", getpid());

    if (mkfifo(str_Pid,0666) == -1) {
        perror ("Fifo Error");
        exit (EXIT_FAILURE);
    }

    // Enviar uma mensagem a sinalizar que é um status (tipo==2)

    int fd_Sinalizar = open("fifo", O_WRONLY);

    Mensagem msg_Sinalizar;
    msg_Sinalizar.type = 2;
    msg_Sinalizar.pid = getpid();
    write(fd_Sinalizar, &msg_Sinalizar, sizeof(Mensagem));

    // Abrir o fifo que vai receber as mensagens do servidor

    int fd = open(str_Pid, O_RDONLY);

    //Calcular o tempo em que o status foi chamado

    struct timeval tempo_Aux;
    gettimeofday(&tempo_Aux, NULL);
    long tempo = tempo_Aux.tv_sec * 1000000L + tempo_Aux.tv_usec;

    // Escrever para o standard output do cliente

    ssize_t bytes;
    struct mensagem msg;
    while ((bytes = read(fd, &msg, sizeof(msg)))>0) {
        char str_Output[200];
        sprintf(str_Output, "%d %s %ld miliseconds\n", msg.pid, msg.name_program, (tempo - msg.tempo) / 1000);
        write(1, &str_Output, strlen(str_Output)+1);
    }

    close(fd);
}

void execute_program(char* program_name, char** program_args)
{
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
    }
    else if (pid_fork == 0) {
        // Fecha o descritor de leitura

        close(pipe_anonimo[0]);

        // Envia o pid para o servidor

        msg_antes.pid = getppid();

        char str_Pid_Message[60];
        sprintf(str_Pid_Message, "Running PID %d\n", msg_antes.pid);
        write (1, &str_Pid_Message, strlen(str_Pid_Message)+1);

        // Envia o nome do programa para o servidor

        strcpy(msg_antes.name_program, program_name);

        // Envia o timestamp atual para o servidor

        msg_antes.type = 1;
        struct timeval tempo_Aux;
        gettimeofday(&tempo_Aux, NULL);
        msg_antes.tempo = tempo_Aux.tv_sec * 1000000L + tempo_Aux.tv_usec;

        write(fd, &msg_antes, sizeof(msg_antes));
        write(pipe_anonimo[1], &msg_antes.tempo, sizeof(msg_antes.tempo));

        // Fecha o descritor de escrita

        close(pipe_anonimo[1]);

        // Executa o programa pedido pelo cliente

        execvp(program_name, program_args);

        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else {
        // Fecha o descritor de escrita
        
        close(pipe_anonimo[1]);

        int status;
        wait(&status);

        struct timeval tempo_Aux;
        gettimeofday(&tempo_Aux, NULL);
        msg_depois.tempo = tempo_Aux.tv_sec * 1000000L + tempo_Aux.tv_usec;
        
        read(pipe_anonimo[0], &start_anonimo, sizeof(msg_antes.tempo));

        // Fecha o descritor de leitura

        close(pipe_anonimo[0]);

        // Envia a mensagem para o servidor
        
        msg_depois.pid = getpid();
        strcpy(msg_depois.name_program, program_name);
        msg_depois.type = 0;    // 0 para identificar que o programa terminou
        write(fd, &msg_depois, sizeof(msg_depois));
    
        long elapsed_microseconds = msg_depois.tempo - start_anonimo.tv_sec;
        
        char str_Output[200];
        sprintf(str_Output ,"Ended in %ld miliseconds\n", elapsed_microseconds / 1000);
        write (1, &str_Output, strlen(str_Output)+1);
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
    else if (argc >= 4 && !strcmp(argv[1], "execute") && !strcmp(argv[2], "-p"))
    {
        char* pipeline = argv[3];
        execute_pipeline(pipeline);
    }
    else if (argc >= 2 && !strcmp(argv[1], "status"))
    {
        get_status();
    }
    else if (argc >= 3 && !strcmp(argv[1], "stats-time"))
    {
        char* program_args[argc - 1];
        for (int i = 2; i < argc; i++) {
            program_args[i - 2] = argv[i];
        }
        program_args[argc - 2] = NULL;

        get_status_time(program_args);
    }
    else if (argc >= 4 && !strcmp(argv[1], "stats-command"))
    {
        char* program_name = argv[2];

        char* program_args[argc - 2];
        for (int i = 3; i < argc; i++) {
            program_args[i - 3] = argv[i];
        }
        program_args[argc - 3] = NULL;

        get_status_command(program_name ,program_args);
    }
    else if (argc >= 3 && !strcmp(argv[1], "stats-uniq"))
    {
        char* program_args[argc - 1];
        for (int i = 2; i < argc; i++) {
            program_args[i - 2] = argv[i];
        }
        program_args[argc - 2] = NULL;

        get_status_uniq(program_args);
    }

    return 0;
}
