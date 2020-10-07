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


#define LEN 4096


void manual();

void server(int porta);

void cliente(int porta , char ip[]);

//---------------------------------------------------------------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------------------------------------------------------------
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
    char mensagem[4096];
    char apelido[31];

    meu_socket = socket(AF_INET, SOCK_STREAM, 0);//criando socket TCP/IP usando IPV4

    //teste pra ver se o socket foi criado com sucesso
    if (meu_socket == -1){
		perror("socket");
		exit(1);
	}
	else{
		printf("socket criado com sucesso\n");
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
    strcpy(mensagem,"seja bem-vindo\n\0");//mensagem a ser enviada no começo da conexão

    printf("Dê um apelido a pessoa que vai se conectar á você : ");
    scanf("%s", apelido);
    //------------------------------------------------------------------------------------------------
    void *receber_msg(){
        while(true){
            if((tamanho_da_resposta = recv(cliente_socket,mensagem,LEN,0)) > 0){
                mensagem[tamanho_da_resposta-1] = '\0';


                time(&segundos);
                data_hora_atual = localtime(&segundos);

                printf("%d:%d:%d| %s -> %s\n",data_hora_atual->tm_hour, data_hora_atual->tm_min, data_hora_atual->tm_sec, apelido, mensagem);
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
    if(send(cliente_socket,mensagem, strlen(mensagem), 0)){//envia a mensagem uma unica vez
		printf("Aguardando resposta...\n");
        pthread_create(&thread_server_receber_msg,NULL, receber_msg, NULL);
        pthread_create(&thread_server_enviar_msg,NULL, enviar_msg, NULL);
        pthread_join(thread_server_receber_msg,NULL);
        pthread_join(thread_server_enviar_msg,NULL);
	}


}
//---------------------------------------------------------------------------------------------------------------------------------------------
void cliente(int porta , char ip[]){
    //struct usadas na parte do cliente
    struct sockaddr_in servidor;
    struct tm *data_hora_atual; //ponteiro para o struct

    time_t segundos; // variável usada para o tempo

    //variáveis usadas na parte do cliente
    int meu_socket;
	int tamanho = sizeof(servidor);
	int tamanho_da_mensagem;
	char mensagem[4096];
    char apelido[31];// o ultimo caracter é o indicador de final da string e pra poder usar 30 caracters coloquei o numero 31

    meu_socket = socket(AF_INET, SOCK_STREAM, 0);//criando socket TCP/IP usando IPV4

	if (meu_socket == -1)
	{
		perror("socket");
		exit(1);
	}
	else
	{
		printf("socket criado com sucesso\n");
	}

    printf("Dê um apelido a pessoa que você ira se conectar : ");
    scanf("%s", apelido);

	servidor.sin_family = AF_INET;// definindo protocolo
	servidor.sin_port = htons(porta); 
	servidor.sin_addr.s_addr = inet_addr(ip);//ip do servidor que desejamos nos conectar
	memset(servidor.sin_zero, 0x0, 8);

    if((connect(meu_socket,(struct sockaddr*)&servidor, tamanho)) == -1){
		perror("connect ");
		exit(1);
	}
    //------------------------------------------------------------------------------------------------
    void *receber_msg(){
        while(true){
            if((tamanho_da_mensagem = recv(meu_socket,mensagem,LEN,0)) > 0){
                mensagem[tamanho_da_mensagem-1] = '\0';

                time(&segundos);
                data_hora_atual = localtime(&segundos);

                printf("%d:%d:%d| %s -> %s\n",data_hora_atual->tm_hour, data_hora_atual->tm_min, data_hora_atual->tm_sec, apelido, mensagem);
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