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

#include "Client.h"

#define LEN 4096


void cliente(int porta , char ip[]){
    //struct usadas na parte do cliente
    struct sockaddr_in servidor;
    struct tm *data_hora_atual; //ponteiro para o struct

    time_t segundos; // variável usada para o tempo

    //variáveis usadas na parte do cliente
    int meu_socket;
	int tamanho = sizeof(servidor);
	int tamanho_da_mensagem;
    short unsigned int tamanho_do_nome_server;
	char mensagem[4096];
    char nome[31];// o ultimo caracter é o indicador de final da string e pra poder usar 30 caracters coloquei o numero 31
    char nome_server[31];


    meu_socket = socket(AF_INET, SOCK_STREAM, 0);//criando socket TCP/IP usando IPV4

	if (meu_socket == -1){
		perror("socket");
		exit(1);
	}
	else{
		printf("conectado com sucesso!!!\n");
	}


	servidor.sin_family = AF_INET;// definindo protocolo
	servidor.sin_port = htons(porta); 
	servidor.sin_addr.s_addr = inet_addr(ip);//ip do servidor que desejamos nos conectar
	memset(servidor.sin_zero, 0x0, 8);

    if((connect(meu_socket,(struct sockaddr*)&servidor, tamanho)) == -1){
		perror("connect ");
		exit(1);
	}

    //enviar nome
    printf("Coloque um nome de usuário para a conversa (esse nome aparecera para o destinatário) -> ");
    scanf("%s",nome);
    send(meu_socket,nome, strlen(nome), 0);

    //receber nome
    if((tamanho_do_nome_server = recv(meu_socket,nome_server,LEN,0)) > 0){
        nome_server[tamanho_do_nome_server] = '\0';
    }

    //------------------------------------------------------------------------------------------------
    void *receber_msg(){
        while(true){
            if((tamanho_da_mensagem = recv(meu_socket,mensagem,LEN,0)) > 0){
                mensagem[tamanho_da_mensagem-1] = '\0';

                time(&segundos);
                data_hora_atual = localtime(&segundos);

                printf("%d:%d:%d| %s -> %s\n",data_hora_atual->tm_hour, data_hora_atual->tm_min, data_hora_atual->tm_sec, nome_server, mensagem);
            }
        }
    }
    //------------------------------------------------------------------------------------------------
    void *enviar_msg(){
        while(true){
            memset(mensagem, 0x0,LEN);
            //scanf("%s", &mensagem);
            fgets(mensagem,LEN,stdin);// codigo que eu usava antes e foi trocado pela linha de cima
            send(meu_socket,mensagem, strlen(mensagem), 0);

        }
    }
    pthread_t thread_cliente_receber_msg, thread_cliente_enviar_msg;

    //------------------------------------------------------------------------------------------------
    pthread_create(&thread_cliente_receber_msg,NULL, receber_msg, NULL);
    pthread_create(&thread_cliente_enviar_msg,NULL, enviar_msg, NULL);
    pthread_join(thread_cliente_receber_msg,NULL);
    pthread_join(thread_cliente_enviar_msg,NULL);
}
