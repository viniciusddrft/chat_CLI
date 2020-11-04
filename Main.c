#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <omp.h>
// bibliotecas para sockets
//#include <netdb.h>  as bibliotecas abaixo substitui essa melhor
#include <sys/types.h>    // AF_INET, SOCK_STREAM
#include <sys/socket.h>   // socket(), connect()
#include <netinet/in.h>   // struct sockaddr_in
#include <arpa/inet.h>    // htons(), inet_addr()
#include <unistd.h>       // close()

#include "Server.h"
#include "Client.h"

#define LEN 4096




void manual();

int main(int argc, char *argv[]){
    //ponteiro para struct que armazena data e hora  
    struct tm *data_hora_atual;    
  
    //variável do tipo time_t para armazenar o tempo em segundos  
    time_t segundos;
  
    //obtendo o tempo em segundos  
    time(&segundos);   
  
    //para converter de segundos para o tempo local utilizamos a função localtime  
    data_hora_atual = localtime(&segundos);  
  
    //para acessar os membros de uma struct usando o ponteiro utilizamos o operador ->

    printf("\nRelógio -> %d:%d:%d\n",data_hora_atual->tm_hour, data_hora_atual->tm_min, data_hora_atual->tm_sec);//hora minuto e segundo

    if (argc==4 && !strcmp("--server",argv[1]) && !strcmp("-p",argv[2])){
        int porta = atoi(argv[3]);
        server(porta);
    }
    else if (argc==5 && !strcmp("--client",argv[1]) && !strcmp("-p",argv[2])){
        int porta = atoi(argv[3]);
        cliente(porta , argv[4]);
    }
    else{
        manual();
    }
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------------
void manual(){
    printf("Exemplo de como usar como servidor ->  ./main --server -p <PORT>\n");
    printf("Exemplo de como usar como cliente -> ./main --client -p <PORT> <IP>\n");
}