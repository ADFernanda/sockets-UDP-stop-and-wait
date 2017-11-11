#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <unistd.h>

#define NUM_PARAMS_SERVIDOR 3
#define NUM_PARAMS_CLIENTE 5

//verifica se portoServidor e tamBuffer são inteiros
int VerificaEntradas(char *portoServidor, char *tamBuffer)
{
    char *p1, *p2;
    errno = 0;
    long conv1 = strtol(portoServidor, &p1, 10);
    long conv2 = strtol(tamBuffer, &p2, 10);

    if (errno != 0 || *p1 != '\0'|| *p2 != '\0' || conv1 > INT_MAX || conv2 > INT_MAX) 
    {
        printf("Erro. Verifique a entrada fornecida.\n");
        return 1;
    }
    else
    {
        return 0;
    }
}
