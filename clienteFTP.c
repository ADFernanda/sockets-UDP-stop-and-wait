#include "lib.h"
#include "tp_socket.h"
#include <arpa/inet.h>


// void RecebeArquivo(){}

int main (int argc, char *argv[]){

    //verifica quantidade de entradas
    if (argc != NUM_PARAMS_CLIENTE)
    {
        printf("Erro. Verrifique os arqumentos passados para servidorFTP.\n");
        return 1;
    }
    //verifica se as entradas numéricas são válidas
    if (VerificaEntradas(argv[2], argv[4]) == 1)
    {
        return 1;
    }

    // inicia interface tp_socket
    if(tp_init() < 0){
        printf("Erro ao iniciar interface tp_socket\n");
        return 1;
    }

    so_addr *remoto = (so_addr*) malloc (sizeof(so_addr));
    int tamBuffer = atoi(argv[4]), portoServidor = atoi(argv[2]), clientefd, len = sizeof(remoto), slen, i=0;
    char *buffer = (char*) calloc (tamBuffer ,sizeof(char)), *hostServidor = argv[1], *nomeArquivo = argv[3];
    FILE *arquivoRecebido = fopen("arquivoRecebido", "w+");
    double taxa = 0;
    unsigned int tempoGastoMs = 0, numBytes = 0;
    double numKbytes = 0;

    char *stringId = (char*) calloc (20 ,sizeof(char)), *str=(char*) calloc (tamBuffer+25 ,sizeof(char));;
    int id = 0, idRecebido = 0;
    char * pch;

    struct timeval  tvInicial, tvFinal;
    gettimeofday(&tvInicial, NULL);
    
    //cria um socket com a porta passada
    clientefd = tp_socket(0);
    if(clientefd < 0){
        printf("Erro ao chamar tp_socket\n");
        return 1;
    }

    if(tp_build_addr(remoto, hostServidor, portoServidor) < 0){
        printf("Erro ao chamar tp_build_addr\n");
        return 1;
    }

    //envia nome arquivo
    //printf("\ntamanho buffer %d\n", tamBuffer);

    //chamar tp_mtu()
    int tamanhoMTU = tp_mtu();
    if( strlen(nomeArquivo) > tamanhoMTU){
        printf("Erro: tamanho do nome do arquivo maior que %d (tp_mtu)\n", tamanhoMTU);
        return 1;
    }

    tp_sendto(clientefd, nomeArquivo, strlen(nomeArquivo), remoto);

    int bytesRecebidos = 0;
    while(1){
        memset(buffer, 0x0, tamBuffer);
        bytesRecebidos = tp_recvfrom(clientefd, buffer, tamBuffer, remoto);
        if(bytesRecebidos > 0){
            printf("%s", buffer);

            // sequencia de caracteres que indica fim do arquivo recebido
            if(strcmp("fechar arquivo: 123456789",buffer) == 0){
                break;
            }else{
                pch = strtok (buffer,"/");
                idRecebido = atoi(pch);
                printf("\nids: %d %d\n\n", idRecebido, id);
                if(idRecebido == id+1){
                    pch = strtok (NULL, "\0");
    
                    fwrite (pch , sizeof(char), strlen(pch), arquivoRecebido);
                    id++;
                }
            }            
        }else{            
            break;
        }           
    }

    fclose(arquivoRecebido);

    return 0;
}