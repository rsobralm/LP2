#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <string>
//#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

using namespace std;


void * recebe_mensagem(void* param) {

	int client_fd = (intptr_t) param;
    //ptr_thread_arg targ = (ptr_thread_arg)param;

	char msg[100];
	
	while(1) {
        bzero(msg, 100); // inicializa a mensagem com 0
        read(client_fd, msg, 100); // le mensagem do socket cliente associado
       // printf("%s enviou uma mensagem: %s\n",targ->nome,msg); // exibe o que recebeu do cliente
        cout  << "\nmensagem recebida: " << msg << endl;

    }

}



int main(int argc, char** argv){

	int client_fd; // file descriptor do socket do cliente

	int port = 22000;

	char host[] = "127.0.0.1"; 
    string client_name = argv[1]; 

	char send_msg[100]; // string com mensagem a enviar
	char recv_msg[100]; // string com mensagem a receber

	struct sockaddr_in server_addr; // struct com informacoes do servidor a conectar

	pthread_t threads;

	client_fd = socket(AF_INET, SOCK_STREAM, 0); //criacao do socket cliente

	bzero(&server_addr, sizeof(server_addr)); // inicializacao da struct que armazenara endereco do servidor

	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(port); // associa porta do servidor a struct
	inet_aton(host, &server_addr.sin_addr); // associa endereco do servidor (armazenado na string host) a struct

	printf("Conectando-se a porta %d do servidor %s\n", port, host);

	connect(client_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)); // funcao bloqueante, conecta ao servidor

    write(client_fd, client_name.c_str(), client_name.size());

	while(1) {
		bzero( send_msg, 100);
		bzero( recv_msg, 100);
		
		printf("Digite algo para enviar ao servidor: ");
		fgets(send_msg, 100, stdin); //le do usuario string para enviar ao servidor

		write(client_fd, send_msg, strlen(send_msg)+1);
		//read(client_fd, recv_msg, 100); // le do servidor string para exibir para o usuario
		pthread_create(&threads, NULL, recebe_mensagem, (void*) (intptr_t)client_fd);

		//printf("Recebi do servidor: %s\n", recv_msg); // exibe o recebido na tela
		

	}	

	return 0;
}
