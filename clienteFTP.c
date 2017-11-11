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
    tp_sendto(clientefd, nomeArquivo, strlen(nomeArquivo), remoto);

    int bytesRecebidos = 0;
    while(1){
        memset(buffer, 0x0, tamBuffer);
        bytesRecebidos = tp_recvfrom(clientefd, buffer, tamBuffer, remoto);
        if(bytesRecebidos > 0){
            printf("%s", buffer);
            if(strcmp("fechar arquivo: 123456789",buffer) == 0){
                break;
            }else{
                fwrite (buffer , sizeof(char), strlen(buffer), arquivoRecebido);
                i++;
            }            
        }else{            
            break;
        }           
    }

    // //calculos para tempo gasto e taxa
    // numBytes = i * tamBuffer;
    // numKbytes = numBytes/1000;
    
    // gettimeofday(&tvFinal, NULL);
    // unsigned int time_in_sec = (tvFinal.tv_sec) -  (tvInicial.tv_sec);
    // unsigned int time_in_mill = (tvFinal.tv_usec / 1000) - (tvInicial.tv_usec / 1000); // convert tv_sec & tv_usec to millisecond

    // if(time_in_mill == 0){
    //     taxa = 0;
    // }
    // else if(time_in_sec == 0){
    //     taxa = (double)numKbytes/time_in_mill;
    //     taxa = taxa*1000;
    // }else{
    //     taxa = (double)numKbytes/time_in_sec;
    // }

    // printf("Buffer = \%5u byte(s), \%10.2f kbps (\%u bytes em \%3u.\%06u s)\n", tamBuffer, taxa, numBytes, time_in_sec, time_in_mill);

    fclose(arquivoRecebido);

    return 0;
}