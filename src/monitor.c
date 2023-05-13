#include "execute.h"
#include "hashTable.h"

int main (int argc, char *argv[]) {

    char *nome_Pasta = argv[1];

    if (mkdir(nome_Pasta, 0777) == -1) {
        return 1;
    }

    if (mkfifo("fifo",0666) == -1) {
        perror ("Fifo Error");
        exit (EXIT_FAILURE);
    }

    int fd = open ("fifo", O_RDWR);

    // Criar a hashTable que irá guardar os programas que estão a ser executados no momento

    HashTable* hashTable = newHashTable();

    // Receber as mensagens dos clientes

    ssize_t bytes;
    struct mensagem msg;

    while ((bytes = read (fd,&msg,sizeof(msg)))>0) {

        if (msg.type==2) {      // status

            char str_Pid[60];
            sprintf(str_Pid, "%d", msg.pid);
            int fd_Status = open (str_Pid, O_WRONLY);

            // Percorrer todos as mensagens da hashTable e enviar uma de cada vez para o fifo

            for (int i = 0; i < MAX_SIZE; i++) {
                HashNode* node = hashTable->nodes[i];
                if (node != NULL) {
                    Mensagem mensagem;
                    strcpy (mensagem.name_program, node->value->name_program);
                    mensagem.pid = node->value->pid;
                    mensagem.tempo = node->value->tempo;
                    mensagem.type = node->value->type;

                    write(fd_Status, &mensagem, sizeof(Mensagem));
                }
            }

            close (fd_Status);
        }
        else if (msg.type==3) {     // Stats-time

            // Criar o fifo que recebe as mensagens  do cliente

            char str_Pid_Receber[60];
            sprintf(str_Pid_Receber, "%d_Tracer_To_Monitor", msg.pid);

            if (mkfifo(str_Pid_Receber,0666) == -1) {
                perror ("Fifo Error");
                exit (EXIT_FAILURE);
            }

            // Abrir o fifo onde o cliente irá receber as mensagens do servidor

            char str_Pid_Enviar[60];
            sprintf(str_Pid_Enviar, "%d_Monitor_To_Tracer", msg.pid);
            int fd_Enviar = open (str_Pid_Enviar, O_WRONLY);

            // Abrir o fifo que irá receber os PIDs dos processos solicitados

            int fd_Receber = open (str_Pid_Receber, O_RDONLY);

            // Devolver os tempos dos PIDs solicitados um a um

            ssize_t bytes;
            int pid;
            int fd_File;
            char file_Name[50];

            while ((bytes = read (fd_Receber, &pid, sizeof(int)))>0) {

                sprintf (file_Name, "%s/%d", nome_Pasta, pid);
                fd_File = open (file_Name, O_RDONLY, 0600);

                if (fd_File!=-1) {

                    Store dados;
                    read (fd_File, &dados, sizeof(Store));
                    write (fd_Enviar, &dados.tempo, sizeof(long));

                    close(fd_File);
                }
            }

            close (fd_Receber);
            close (fd_Enviar);
        }
        else if (msg.type==4) {     // Stats-command

            // Criar o fifo que recebe as mensagens  do cliente

            char str_Pid_Receber[60];
            sprintf(str_Pid_Receber, "%d_Tracer_To_Monitor", msg.pid);

            if (mkfifo(str_Pid_Receber,0666) == -1) {
                perror ("Fifo Error");
                exit (EXIT_FAILURE);
            }

            // Abrir o fifo onde o cliente irá receber as mensagens do servidor

            char str_Pid_Enviar[60];
            sprintf(str_Pid_Enviar, "%d_Monitor_To_Tracer", msg.pid);
            int fd_Enviar = open (str_Pid_Enviar, O_WRONLY);

            // Abrir o fifo que irá receber os PIDs dos processos solicitados

            int fd_Receber = open (str_Pid_Receber, O_RDONLY);

            ssize_t bytes;
            int pid;
            int fd_File;
            char file_Name[50];
            int program_Found = 1;

            while ((bytes = read (fd_Receber, &pid, sizeof(int)))>0) {

                sprintf (file_Name, "%s/%d", nome_Pasta, pid);
                fd_File = open (file_Name, O_RDONLY, 0600);

                if (fd_File!=-1) {

                    Store dados;
                    read (fd_File, &dados, sizeof(Store));

                    if (!strcmp(dados.name_program, msg.name_program)) {
                        write (fd_Enviar, &program_Found, sizeof(int));
                    }

                    close(fd_File);
                }
            }

            close (fd_Receber);
            close (fd_Enviar);
        }
        else if (msg.type==5) {     // Stats-uniq

            // Criar o fifo que recebe as mensagens  do cliente

            char str_Pid_Receber[60];
            sprintf(str_Pid_Receber, "%d_Tracer_To_Monitor", msg.pid);

            if (mkfifo(str_Pid_Receber,0666) == -1) {
                perror ("Fifo Error");
                exit (EXIT_FAILURE);
            }

            // Abrir o fifo onde o cliente irá receber as mensagens do servidor

            char str_Pid_Enviar[60];
            sprintf(str_Pid_Enviar, "%d_Monitor_To_Tracer", msg.pid);
            int fd_Enviar = open (str_Pid_Enviar, O_WRONLY);

            // Abrir o fifo que irá receber os PIDs dos processos solicitados

            int fd_Receber = open (str_Pid_Receber, O_RDONLY);

            // Criar um array para veririfar se o nome do programa já foi escrito

            int size = 20;
            int sp = 0;
            char **nomes_Programas = malloc(size*sizeof(char*));

            for (int i = 0;i<size;i++) {
                nomes_Programas[i] = NULL;
            }

            // Enviar os programas únicos

            ssize_t bytes;
            int pid;
            int fd_File;
            char file_Name[50];

            while ((bytes = read (fd_Receber, &pid, sizeof(int)))>0) {

                sprintf (file_Name, "%s/%d", nome_Pasta, pid);
                fd_File = open (file_Name, O_RDONLY, 0600);

                if (fd_File!=-1) {

                    Store dados;
                    read (fd_File, &dados, sizeof(Store));

                    // Verifica se o programa já foi escrito

                    int i;
                    for (i = 0;i<sp && strcmp(nomes_Programas[i], dados.name_program)!=0;i++);

                    if (i==sp) {
                        if (sp==size) {
                            nomes_Programas = realloc (nomes_Programas, 2*size*sizeof(char*));
                            size *= 2;
                        }

                        for (int j = size/2;j<size;j++) {
                            nomes_Programas[j]=NULL;
                        }

                        nomes_Programas[i] = strdup(dados.name_program);
                        sp++;

                        // Envia o nome do programa para o cliente

                        write (fd_Enviar, &dados.name_program, 60*sizeof(char));
                    }

                    close(fd_File);
                }
            }

            for (int i = 0;i<sp;i++) {
                free(nomes_Programas[i]);
            }
            free(nomes_Programas);

            close (fd_Receber);
            close (fd_Enviar);
        }
        else {

            // Verificar se o processo já foi iniciado

            Mensagem* msg_HashTable = get (hashTable, msg.pid);
            if (msg_HashTable==NULL) {
                insert (hashTable, msg.pid, msg);
            }
            else {
                // Cria o ficheiro com as informações do programa que acabou de executar

                // Criar o ficheiro

                char nome_Ficheiro[200];
                sprintf (nome_Ficheiro, "%s/%d", nome_Pasta, msg.pid);
                int fd_Output = open(nome_Ficheiro, O_CREAT | O_WRONLY, 0600);

                // Inserir as informações do programa executado

                Store dados;
                strcpy (dados.name_program, msg.name_program);
                dados.tempo = (msg.tempo - msg_HashTable->tempo) / 1000;
                write (fd_Output, &dados, sizeof(Store));

                close (fd_Output);
                delete (hashTable, msg.pid);
            }

        }

    }

    close(fd);

    return 0;
}
