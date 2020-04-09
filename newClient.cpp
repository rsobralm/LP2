#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

using namespace std;

class Client {
  int client_fd;
  string client_name;

  public: 
  Client();
  void getMessage();
  void setClient_fd(int client_fd);
  void setClient_name(string client_name);
  string getClient_name();
  int getClient_fd();
};

Client::Client(){};

void Client::getMessage(){

    char msg[300];
	
	while(1){
        bzero(msg, 300); // inicializa a mensagem com 0
        read(client_fd, msg, 300); // le mensagem do socket cliente associado
       // printf("%s enviou uma mensagem: %s\n",targ->nome,msg); // exibe o que recebeu do cliente
        cout << "\n" << msg << endl;
    }
}

void Client::setClient_fd(int client_fd){
    this->client_fd = client_fd;
}

void Client::setClient_name(string client_name){
    this->client_name = client_name;
}

string Client::getClient_name(){
	return client_name;
}

int Client::getClient_fd(){
	return client_fd;
}



int main(int argc, char** argv){

    int client_fd; // file descriptor do socket do cliente

	int port = 22000;

	Client client_obj;

	char host[] = "127.0.0.1"; 
   	client_obj.setClient_name(argv[1]); 

	char send_msg[300]; // string com mensagem a enviar
	char recv_msg[300]; // string com mensagem a receber

	struct sockaddr_in server_addr; // struct com informacoes do servidor a conectar

	//pthread_t threads;

	client_obj.setClient_fd(socket(AF_INET, SOCK_STREAM, 0)); //criacao do socket cliente

	bzero(&server_addr, sizeof(server_addr)); // inicializacao da struct que armazenara endereco do servidor

	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(port); // associa porta do servidor a struct
	inet_aton(host, &server_addr.sin_addr); // associa endereco do servidor (armazenado na string host) a struct

    cout << "Conectando-se a porta " << port << " do servidor " << host << endl;

	if(connect(client_obj.getClient_fd(), (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1){ // funcao bloqueante, conecta ao servidor
		cout << "Não foi possivel estabelecer conexão com o servidor, tente novamente" << endl;
		return 0;
	}
	
	write(client_obj.getClient_fd(), client_obj.getClient_name().c_str(), client_obj.getClient_name().size());

	thread t(&Client::getMessage, &client_obj);
	
	while(1) {
		bzero( send_msg, 300);
		bzero( recv_msg, 300);

        cout << "Digite algo para enviar ao servidor: ";
		fgets(send_msg, 300, stdin); //le do usuario string para enviar ao servidor

		write(client_obj.getClient_fd(), send_msg, strlen(send_msg)+1);
		//read(client_fd, recv_msg, 300); // le do servidor string para exibir para o usuario
		//pthread_create(&threads, NULL, recebe_mensagem, (void*) (intptr_t)client_fd);
        //thread t(&Client::getMessage, &client_obj);

		//printf("Recebi do servidor: %s\n", recv_msg); // exibe o recebido na tela
		

	}	

	return 0;

}