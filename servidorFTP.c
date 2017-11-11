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
    char *buffer = (char*) calloc (tamBuffer ,sizeof(char)), *nomeArquivo = (char*) calloc (256 ,sizeof(char));
    FILE *arquivo;

    // setando cabeçalho do pacote
    Pacote *pacote = (Pacote*) malloc (sizeof(Pacote));
    pacote.buffer = (char*) calloc (tamBuffer ,sizeof(char));
    pacote.id = 1;

    //cria um socket com a porta portoServidor
    servidorfd = tp_socket(portoServidor);    
    if(servidorfd < 0){
        printf("Erro ao chamar tp_socket\n");
        return 1;
    }

    // recebe o nome do arquivo
    tp_recvfrom(servidorfd, nomeArquivo, 256, cliente);
    arquivo = fopen(nomeArquivo, "r");
    if(!arquivo){
        perror("fopen");
        return -1;
    }

    //loop para transferência de dados entre servidor e cliente
    size_t bytesLidos = 0;
    int ack = 1; //ack = 1 -> pacote enviado com sucesso, ack = 0 -> pacote não enviado com sucesso

    struct timeval  tvEnvio, tvAposEnvio;
    gettimeofday(&tvInicial, NULL);

    while (!feof(arquivo)) {
        memset(pacote.buffer, 0x0, tamBuffer);

        id(ack == 1){
            bytesLidos = fread(pacote.buffer, tamBuffer, 1, arquivo);
            pacote.id++;
    
            //chamar tp_mtu()
    
            if(tp_sendto(servidorfd, buffer, tamBuffer, cliente) < 0){
                printf("Erro ao enviar dados\n");
                return 1;
            }
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