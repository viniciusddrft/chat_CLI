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

#define LEN 4096


void server(int porta){
    //struct usadas na parte do servidor
    struct sockaddr_in localhost;
    struct sockaddr_in cliente;
    struct tm *data_hora_atual;//ponteiro para o struct

    time_t segundos; // variável usada para o tempo

    //variáveis usadas na parte do servidor
    int cliente_socket;
    int meu_socket;
    int tamanho = sizeof(cliente);
    int tamanho_da_resposta;
    short unsigned int tamanho_do_nome_cliente;
    char mensagem[4096];
    char nome[31];// o ultimo caracter é o indicador de final da string e pra poder usar 30 caracters coloquei o numero 31
    char nome_cliente[31];

    meu_socket = socket(AF_INET, SOCK_STREAM, 0);//criando socket TCP/IP usando IPV4

    //teste pra ver se o socket foi criado com sucesso
    if (meu_socket == -1){
		perror("socket");
		exit(1);
	}
	else{
		printf("Aguardando conexão...\n");
	}

    localhost.sin_family = AF_INET;// definindo protocolo
	localhost.sin_port = htons(porta);
	memset(localhost.sin_zero, 0x0, 8);

    if (bind(meu_socket,(struct sockaddr*)&localhost,sizeof(localhost)) == -1){// atribui o socket a uma porta
		perror("bind ");
		exit(1);
	}
    listen(meu_socket,1);

	if((cliente_socket = accept(meu_socket,(struct sockaddr*)&cliente, &tamanho)) == -1){//aceita a conexão do cliente e se der erro ele já trata
		perror("accept ");
		exit(1);
	}

    //receber nome
    if((tamanho_do_nome_cliente = recv(cliente_socket,nome_cliente,LEN,0)) > 0){
        nome_cliente[tamanho_do_nome_cliente] = '\0';
    }

    printf("Conexão feita com sucesso!!!\n");

    //enviar nome
    printf("Coloque um nome de usuário para a conversa (esse nome aparecera para o destinatário) -> ");
    scanf("%s", nome);
    (send(cliente_socket,nome, strlen(nome), 0));
    //------------------------------------------------------------------------------------------------
    void *receber_msg(){
        while(true){
            if((tamanho_da_resposta = recv(cliente_socket,mensagem,LEN,0)) > 0){
                mensagem[tamanho_da_resposta-1] = '\0';

                time(&segundos);
                data_hora_atual = localtime(&segundos);

                printf("%d:%d:%d| %s -> %s\n",data_hora_atual->tm_hour, data_hora_atual->tm_min, data_hora_atual->tm_sec, nome_cliente, mensagem);
            }
        }
    }
    //------------------------------------------------------------------------------------------------
    void *enviar_msg(){
        while(true){;
            // scanf pega o texto ate achar uma palavra em branco apenas por isso ele só pega uma palavra
            fgets(mensagem,LEN,stdin);//diferente do scanf eu consigo com essa função mandar mensagens com mais de uma palavra;
            (send(cliente_socket,mensagem, strlen(mensagem), 0));

        }
    }
    pthread_t thread_server_receber_msg, thread_server_enviar_msg;
    //------------------------------------------------------------------------------------------------
    pthread_create(&thread_server_receber_msg,NULL, receber_msg, NULL);
    pthread_create(&thread_server_enviar_msg,NULL, enviar_msg, NULL);
    pthread_join(thread_server_receber_msg,NULL);
    pthread_join(thread_server_enviar_msg,NULL);


}