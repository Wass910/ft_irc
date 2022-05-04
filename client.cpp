#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>

typedef struct User{
	char nom[30];
	int age;
}User;

int main(void){
	int socketClient = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addrClient;
	addrClient.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrClient.sin_family = AF_INET;
	addrClient.sin_port = htons(30000);
	connect(socketClient, (const struct sockaddr *)&addrClient, sizeof(addrClient));
	std::cout << "connected." << std::endl;

	char msg[500];
	recv(socketClient, msg, 500, 0);
	std::cout << msg << std::endl;
	std::string temp;
	getline(std::cin, temp);
	int len = 500;
	char msg_send[len];
	int i = 0;
	while(temp[i])
	{
		msg_send[i] = temp[i];
		i++;
	}
	msg_send[i] = '\0';
	send(socketClient, msg_send, len, 0);
	close(socketClient);

	return 0;
}
