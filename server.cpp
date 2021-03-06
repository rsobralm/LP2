#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>


using namespace std;


typedef struct {
    int cliente_fd;
    string nome;
}thread_arg, *ptr_thread_arg;


pthread_t threads[10];
thread_arg arguments[10];

void * recebe_mensagem(void* param) {

	//int client_fd = (intptr_t) param;
    ptr_thread_arg targ = (ptr_thread_arg)param;

	char msg[100];
	
	while(1) {
        bzero(msg, 100); // inicializa a mensagem com 0
        read(targ->cliente_fd, msg, 100); // le mensagem do socket cliente associado
       // printf("%s enviou uma mensagem: %s\n",targ->nome,msg); // exibe o que recebeu do cliente
        cout << targ->nome << " enviou uma mensagem: " << msg << endl;
        //write(targ->cliente_fd, msg, strlen(msg)+1); // envia de volta a mesma mensgem
        for(int i = 0; i < 10; i++){
            if(arguments[i].nome != targ->nome && arguments[i].cliente_fd > 0){
                write(arguments[i].cliente_fd, msg, strlen(msg)+1);
            }
        }

    }

}



int main(int argc, char ** argv){

    //int server_port = atoi(argv[1]);
    int server_port = 22000;
    string server_name = argv[2]; 


    int listen_fd, client_fd; // dois file descriptors, 1 para ouvir solicitacoes, outro para o cliente

    struct sockaddr_in server_addr; // struct com informacoes para o server socket
	struct sockaddr_in client_addr; // struct que armazenara informacoes do cliente conectado

   // pthread_t threads[10]; // array que armazenara 10 threads (MAXIMO DE CLIENTES)

    int thread_count = 0; // contador de threads (de clientes

    cout << "Iniciando Bate-Papo: "  << server_name << " na porta: " << server_port << endl;

    printf("Loading...\n");

	listen_fd = socket(AF_INET, SOCK_STREAM, 0); // listen_fd representa o socket que aguardara requisicoes

	bzero(&server_addr, sizeof(server_addr)); // Inicializa a estrutura do servidor sockaddr_in com 0
	bzero(&client_addr, sizeof(client_addr)); // Inicializa a estrutura do cliente sockaddr_in com 0

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY; // Constante sinaliza que o socket sera um servidor
	server_addr.sin_port = htons(server_port); // Porta a ser associada ao socket servidor criado

	bind(listen_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)); // associa parametros definidos

    listen(listen_fd, 10); // sinaliza que aguardara conexoes na porta associada

    int c = sizeof(struct sockaddr_in);

    printf("Aguardando conexoes na porta %d\n",server_port);

    char name[100];

    while(1){
		client_fd = accept(listen_fd, (struct sockaddr*) &client_addr, (socklen_t*)&c); // funcao bloqueante, gera novo socket 
        arguments[thread_count].cliente_fd = client_fd;
        bzero(name, 100); // inicializa a mensagem com 0
        read(client_fd, name, 100); // le mensagem do socket cliente associado
        cout << name << " se conectou com o IP: " <<  inet_ntoa(client_addr.sin_addr) << endl; // exibe o que recebeu do cliente
        arguments[thread_count].nome = name;
		pthread_create(&threads[thread_count], NULL, recebe_mensagem, &(arguments[thread_count++]));
        //thread_count++;

	}

    return 0;
}