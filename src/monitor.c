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

        }
        else if (msg.type==3) {     // Stats-command

        }
        else if (msg.type==4) {     // Stats-uniq

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
                sprintf (nome_Ficheiro, "%s/%d.txt", nome_Pasta, msg.pid);
                int fd_Output = open(nome_Ficheiro, O_CREAT | O_WRONLY, 0600);

                // Inserir as informações do programa executado

                char output[300];
                sprintf (output, "%s\n%ld", msg.name_program, (msg.tempo - msg_HashTable->tempo) / 1000);
                write (fd_Output, &output, strlen(output));

                delete (hashTable, msg.pid);
            }

        }

    }

    close(fd);

    return 0;
}
