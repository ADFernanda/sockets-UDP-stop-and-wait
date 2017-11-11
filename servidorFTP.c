#include "lib.h"
#include "tp_socket.h"

void EnviaArquivo(){}

int main (int argc, char *argv[]){

    //verifica quantidade de entradas
    if (argc != NUM_PARAMS_SERVIDOR)
    {
        printf("Erro. Verrifique os arqumentos passados para servidorFTP.\n");
        return 1;
    }
    //verifica se as entradas numéricas são válidas
    if (VerificaEntradas(argv[1], argv[2]) == 1)
    {
        return 1;
    }

    // inicia interface tp_socket
    if(tp_init() < 0){
        printf("Erro oa iniciar interface tp_socket\n");
        return 1;
    }

    so_addr *servidor = (so_addr*) malloc (sizeof(so_addr)), *cliente = (so_addr*) malloc (sizeof(so_addr));
    int tamBuffer = atoi(argv[2]), portoServidor = atoi(argv[1]), servidorfd, clientefd,
        sizeCliente = sizeof(cliente), slen;
    char *buffer = (char*) calloc (tamBuffer ,sizeof(char)), *nomeArquivo = (char*) calloc (256 ,sizeof(char)),*ack = (char*) calloc (1 ,sizeof(char));
    FILE *arquivo;
    char *stringId = (char*) calloc (20 ,sizeof(char)), *str=(char*) calloc (tamBuffer+25 ,sizeof(char));;
    int id = 1;

    //cria um socket com a porta portoServidor
    servidorfd = tp_socket(portoServidor);    
    if(servidorfd < 0){
        printf("Erro ao chamar tp_socket\n");
        return 1;
    }

    tp_recvfrom(servidorfd, nomeArquivo, 256, cliente);

    arquivo = fopen(nomeArquivo, "r");
    if(!arquivo){
        perror("fopen");
        return -1;
    }
    int a = 0;
    //loop para transferência de dados entre servidor e cliente
    size_t bytesLidos = 0;
int teste;
    sprintf (ack,"0");
    while (!feof(arquivo)) {

        //chamar tp_mtu()

        scanf("%d", &teste);

        if(strcmp("0",ack) == 0){// ack recebido
            memset(buffer, 0x0, tamBuffer);
            memset(str, 0x0, tamBuffer+25);
            
            sprintf (stringId,"%d",id);
            strcat( str, stringId);
            strcat( str, "/");
            
            bytesLidos = fread(buffer, tamBuffer - strlen(str), 1, arquivo);
            
            strcat( str, buffer);
            if(tp_sendto(servidorfd, str, tamBuffer, cliente) < 0){
                printf("Erro ao enviar dados\n");
                return 1;
            }
            id++;
        }else{
            if(tp_sendto(servidorfd, str, tamBuffer, cliente) < 0){
                printf("Erro ao enviar dados\n");
                return 1;
            }
        }
    
        //espera ACK
        if(tp_recvfrom(servidorfd, ack, 1, cliente) < 0){
            printf("Erro ao receber ACK/NACK dados\n");
            return 1;
        }
    }

    if(tp_sendto(servidorfd, "fechar arquivo: 123456789", 26, cliente) < 0){
        printf("Erro ao enviar dados de fechamento do arquivo\n");
        return 1;
    }

    // fclose(arquivo);
    // free(buffer);
    // close(servidorfd);
    // close(clientefd);
    return 0;
}