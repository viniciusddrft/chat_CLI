#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
// bibliotecas para sockets
#include <netdb.h>//só precisa dessa

#define LEN 4096



void server(){
    //struct usadas na parte do servidor
    struct sockaddr_in localhost;
    struct sockaddr_in cliente;
    struct tm *data_hora_atual;//ponteiro para o struct

    //constantes usadas na parte do servidor
    const int PORTA = 3000;

    time_t segundos; // variável usada para o tempo

    //variáveis usadas na parte do servidor
    int cliente_socket;
    int meu_socket;
    int tamanho = sizeof(cliente);
    int tamanho_da_resposta;
    char mensagem[4096];
    char apelido[31];

    meu_socket = socket(AF_INET, SOCK_STREAM, 0);

    //teste pra ver se o socket foi criado com sucesso
    if (meu_socket == -1){
		perror("socket");
		exit(1);
	}
	else{
		printf("socket criado com sucesso\n");
	}

    localhost.sin_family = AF_INET;// definindo protocolo
	localhost.sin_port = htons(PORTA);
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
    void receber_msg(){
 		if((tamanho_da_resposta = recv(cliente_socket,mensagem,LEN,0)) > 0){
            mensagem[tamanho_da_resposta-1] = '\0';

            if (!strcmp(mensagem,"exit")){//codigo para encerar o servidor quando o cliente encerar o socket dele
                close(cliente_socket);//refatorar
            }

            time(&segundos);
            data_hora_atual = localtime(&segundos);

            printf("%d:%d:%d| %s -> %s\n",data_hora_atual->tm_hour, data_hora_atual->tm_min, data_hora_atual->tm_sec, apelido, mensagem);
        }
    }
    //------------------------------------------------------------------------------------------------
    void enviar_msg(){
        printf("%d:%d:%d| Você -> ",data_hora_atual->tm_hour, data_hora_atual->tm_min, data_hora_atual->tm_sec);
        // scanf pega o texto ate achar uma palavra em branco apenas por isso ele só pega uma palavra
        fgets(mensagem,LEN,stdin);//diferente do scanf eu consigo com essa função mandar mensagens com mais de uma palavra;
        (send(cliente_socket,mensagem, strlen(mensagem), 0));
        if (!strcmp(mensagem,"exit")){//refatorar  
            close(cliente_socket);
        }
    }
    //------------------------------------------------------------------------------------------------
    if(send(cliente_socket,mensagem, strlen(mensagem), 0)){//envia a mensagem uma unica vez
		printf("Aguardando resposta...\n");
		while(true){//começo do loop de mensagens
            receber_msg();
            enviar_msg();
		}
	}
    close(meu_socket);
	printf("Servidor Encerrado\n");
}
//---------------------------------------------------------------------------------------------------------------------------------------------
void cliente(){
    //struct usadas na parte do cliente
    struct sockaddr_in servidor;
    struct tm *data_hora_atual; //ponteiro para o struct

    //constantes usadas na parte do cliente
    const int PORTA = 3000;

    time_t segundos; // variável usada para o tempo

    //variáveis usadas na parte do cliente
    int meu_socket;
	int tamanho = sizeof(servidor);
	int tamanho_da_mensagem;
	char mensagem[4096];
    char apelido[31];// o ultimo caracter é o indicador de final da string e pra poder usar 30 caracters coloquei o numero 31

    meu_socket = socket(AF_INET, SOCK_STREAM, 0);

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
	servidor.sin_port = htons(PORTA); 
	servidor.sin_addr.s_addr = inet_addr("127.0.0.1");//ip do servidor que desejamos nos conectar
	memset(servidor.sin_zero, 0x0, 8);

    if((connect(meu_socket,(struct sockaddr*)&servidor, tamanho)) == -1){
		perror("connect ");
		exit(1);
	}
    //------------------------------------------------------------------------------------------------
    void receber_msg(){
        if((tamanho_da_mensagem = recv(meu_socket,mensagem,LEN,0)) > 0){
		    mensagem[tamanho_da_mensagem-1] = '\0';
            if (!strcmp(mensagem,"exit")){//refatorar
                printf("Cliente Encerrado\n");
	            close(meu_socket);
            }    

            time(&segundos);
            data_hora_atual = localtime(&segundos);

			printf("%d:%d:%d| %s -> %s\n",data_hora_atual->tm_hour, data_hora_atual->tm_min, data_hora_atual->tm_sec, apelido, mensagem);
		}
    }
    //------------------------------------------------------------------------------------------------
    void enviar_msg(){
        memset(mensagem, 0x0,LEN);
        printf("%d:%d:%d| Você -> ",data_hora_atual->tm_hour, data_hora_atual->tm_min, data_hora_atual->tm_sec);
        //scanf("%s", &mensagem);
        fgets(mensagem,LEN,stdin);// codigo que eu usava antes e foi trocado pela linha de cima
        send(meu_socket,mensagem, strlen(mensagem), 0);
        if (!strcmp(mensagem,"exit\n")){//refatorar tem 4 comparações iguais a essa
            printf("Cliente Encerrado\n");
	        close(meu_socket);
        }
    }
    //------------------------------------------------------------------------------------------------
    while(true){
        receber_msg();
        enviar_msg();
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[]){
    int escolha;

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

    printf("Olá seja bem-vindo ao meu software de comunicação por terminal\n");
    printf("\n");
    printf("você pode se conectar a um servidor ou ser o servidor para alguém se conectar a você\n");
    printf("Digite 1 para ser o servidor\n");
    printf("Digite 2 para se conectar a um servidor\n");
    scanf("%d", &escolha);
    if (escolha == 1){
        server();
    }
    else if (escolha == 2){
        cliente();
    }
    else{
        printf("escolha invalida\n");
    }
    return 0;
}