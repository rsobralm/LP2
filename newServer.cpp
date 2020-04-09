#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>

using namespace std;

class ServerThreads {
  int client_fd;
  string client_name;

  public: 
  ServerThreads();
  void getMessage(ServerThreads *sv_threads);
  void setClient_fd(int client_fd);
  void setClient_name(string client_name);
  int getClient_fd();
  string getClient_name();
};

ServerThreads::ServerThreads(){};

void ServerThreads::setClient_fd(int client_fd){
    this->client_fd = client_fd;
}

void ServerThreads::setClient_name(string client_name){
    this->client_name = client_name;
}

int ServerThreads::getClient_fd(){
    return client_fd;
}

string ServerThreads::getClient_name(){
    return client_name;
}


void ServerThreads::getMessage(ServerThreads *sv_threads){
    char msg[300];
    char nome_msg[300];
    
    string display = client_name + " enviou: ";
    char cstr[300];
    bzero(cstr, 300);
	strcpy(cstr, display.c_str());

	while(1) {
        bzero(msg, 300); // inicializa a mensagem com 0
        if(read(client_fd, msg, 300)) { // le mensagem do socket cliente associado
            cout << "\n" <<client_name << " enviou uma mensagem: " << msg << endl;
            for(int i = 0; i < 10; i++){
                if(sv_threads[i].client_name != client_name && sv_threads[i].client_fd > 0){
                    strcat(cstr, msg);
                    write(sv_threads[i].client_fd, cstr, strlen(cstr)+1);
                }
            }
        }
        else{
            cout << client_name << " se desconectou do servidor" << endl;
            return;
        }
        
    }

}

    thread clients_threads[10];
    ServerThreads threads[10];

int main(int argc, char** argv){

    int server_port = 22000;
    string server_name = argv[2]; 

    int listen_fd, client_fd; // dois file descriptors, 1 para ouvir solicitacoes, outro para o cliente

    struct sockaddr_in server_addr; // struct com informacoes para o server socket
	struct sockaddr_in client_addr; // struct que armazenara informacoes do cliente conectado

    int thread_count = 0; // contador de threads (de clientes)

    cout << "Iniciando Bate-Papo: "  << server_name << " na porta: " << server_port << endl;
    cout << "Loading..." << endl;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0); // listen_fd representa o socket que aguardara requisicoes

    bzero(&server_addr, sizeof(server_addr)); // Inicializa a estrutura do servidor sockaddr_in com 0
	bzero(&client_addr, sizeof(client_addr)); // Inicializa a estrutura do cliente sockaddr_in com 0

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY; // Constante sinaliza que o socket sera um servidor
	server_addr.sin_port = htons(server_port); // Porta a ser associada ao socket servidor criado

	bind(listen_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)); // associa parametros definidos

    listen(listen_fd, 10); // sinaliza que aguardara conexoes na porta associada

    int c = sizeof(struct sockaddr_in);
    char name[300];

    cout << "Aguardando conexoes na porta " << server_port << endl;

    while(1){
		client_fd = accept(listen_fd, (struct sockaddr*) &client_addr, (socklen_t*)&c); // funcao bloqueante, gera novo socket 
        threads[thread_count].setClient_fd(client_fd);
        bzero(name, 300); // inicializa a mensagem com 0
        read(client_fd, name, 300); // le mensagem do socket cliente associado
        cout << name << " se conectou com o IP: " <<  inet_ntoa(client_addr.sin_addr) << endl; // exibe o que recebeu do cliente
		//pthread_create(&threads[thread_count], NULL, recebe_mensagem, &(arguments[thread_count++]));
        threads[thread_count].setClient_name(name);
        clients_threads[thread_count] = thread(&ServerThreads::getMessage, &threads[thread_count++], threads);

	}


    return 0;
}