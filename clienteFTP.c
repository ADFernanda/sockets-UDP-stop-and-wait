#include "lib.h"
#include "tp_socket.h"

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
    int tamBuffer = atoi(argv[4]) + 11, portoServidor = atoi(argv[2]), clientefd;
    char *buffer = (char*) calloc (tamBuffer ,sizeof(char)), *hostServidor = argv[1], *nomeArquivo = argv[3], *ack = "0", *nack = "1";
    FILE *arquivoRecebido = fopen(nomeArquivo, "w+");
    double taxa = 0;
    unsigned int numBytes = 0;
    double numKbytes = 0;

    char *stringId = (char*) calloc (20 ,sizeof(char)), *str=(char*) calloc (tamBuffer+25 ,sizeof(char));;
    int id = 0, idRecebido = 0;
    char * strtokBuffer;    

    struct timeval  tvInicial, tvFinal;
    gettimeofday(&tvInicial, NULL);

    //verifica se tamanho do buffer é compatível com a MTU
    if(tp_mtu() < tamBuffer){
        printf("Erro: buffer (buffer informado + 11 bytes de cabeçalho) maior que MTU");
        return 1;
    }
    
    //cria um socket com a porta passada
    clientefd = tp_socket(0);
    if(clientefd < 0){
        printf("Erro ao chamar tp_socket\n");
        return 1;
    }

    // configura servidor
    if(tp_build_addr(remoto, hostServidor, portoServidor) < 0){
        printf("Erro ao chamar tp_build_addr\n");
        return 1;
    }

    // verifica se o nome do arquivo é maior que MTU
    if( strlen(nomeArquivo) > tp_mtu()){
        printf("Erro: tamanho do nome do arquivo maior que MTU (tp_mtu)\n");
        return 1;
    }

    //envia nome do arquivo
    tp_sendto(clientefd, nomeArquivo, strlen(nomeArquivo), remoto);

    // configura timeout
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(clientefd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
        perror("Error");
    }

    // while para recebimento do arquivo
    int bytesRecebidos = 0;
    
    while(1){
        memset(buffer, 0x0, tamBuffer);
        bytesRecebidos = tp_recvfrom(clientefd, buffer, tamBuffer, remoto);

        if(bytesRecebidos > 0){            
            
            if(strcmp("0",buffer) == 0){// sequencia de caracteres que indica fim do arquivo recebido
                break;
            }else{
                strtokBuffer = strtok (buffer,"/");
                idRecebido = atoi(strtokBuffer);
                if(idRecebido == id+1){ // verifica se a ordem dos dados recebidos está correta e caso positivo, escreve dados no arquivo
                    
                    strtokBuffer = strtok (NULL, "\0");
                    numBytes += (double)strlen(strtokBuffer);
                    fwrite (strtokBuffer , sizeof(char), strlen(strtokBuffer), arquivoRecebido);
                    id++;

                    if( tp_sendto(clientefd, ack, strlen(ack), remoto) < 0){
                        printf("Erro ao enviar ACK\n");
                        return 1;
                    }
                } else{ //envia NACK ao receber dados fora de ordem                   
                    if( tp_sendto(clientefd, nack, strlen(nack), remoto) < 0){
                        printf("Erro ao enviar NACK\n");
                        return 1;
                    }
                }
            }            
        }else{//envia NACK ao ocorrer timeout             
            if( tp_sendto(clientefd, nack, strlen(nack), remoto) < 0){
                printf("Erro ao enviar NACK\n");
                return 1;
            }
        }            
    }

    //calculos para tempo gasto e taxa
    numKbytes = (double)numBytes/1000;
    
    gettimeofday(&tvFinal, NULL);
    unsigned int time_in_sec = (tvFinal.tv_sec) -  (tvInicial.tv_sec);
    unsigned int time_in_mill = (tvFinal.tv_usec / 1000) - (tvInicial.tv_usec / 1000); // convert tv_sec & tv_usec to millisecond

    if(time_in_mill == 0){
        taxa = numBytes;
    }
    else if(time_in_sec == 0){
        taxa = (double)numKbytes*1000;
    }else{
        taxa = (double)numKbytes/time_in_sec;
    }

    printf("Buffer = \%5u byte(s), \%10.2f kbps (\%u bytes em \%3u.\%06u s)\n", atoi(argv[4]), taxa, numBytes, time_in_sec, time_in_mill);

    fclose(arquivoRecebido);
    free(remoto);
    free(buffer);
    free(stringId);
    free(str);

    return 0;
}